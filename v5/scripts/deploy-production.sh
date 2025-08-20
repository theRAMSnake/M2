#!/bin/bash

# Materia V5 - Production Deployment Script
# This script deploys the application for production with HTTPS

set -e

echo "🚀 Starting Materia V5 Production Deployment..."

# Configuration
APP_NAME="materia-v5-production"
HTTP_PORT=80
HTTPS_PORT=443
DATA_DIR="/materia"
LOGS_DIR="/materia/logs"
BACKUPS_DIR="/materia/backups"
MATERIA_DIR="/materia"
SSL_DIR="/materia/ssl"
OLD_DB_PATH="/materia/legacy/database.db"  # Path to old database

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running as root
check_root() {
    if [ "$EUID" -ne 0 ]; then
        print_error "This script must be run as root for production deployment"
        print_status "Please run: sudo $0"
        exit 1
    fi
}

# Check prerequisites
check_prerequisites() {
    print_status "Checking prerequisites..."
    
    if ! command -v docker &> /dev/null; then
        print_error "Docker is not installed"
        exit 1
    fi
    
    if ! command -v docker-compose &> /dev/null; then
        print_error "Docker Compose is not installed"
        exit 1
    fi
    
    if ! command -v nginx &> /dev/null; then
        print_warning "Nginx is not installed - will install automatically"
    fi
    
    if ! command -v certbot &> /dev/null; then
        print_warning "Certbot is not installed - will install automatically"
    fi
    
    print_success "Prerequisites check passed"
}

# Install required packages
install_packages() {
    print_status "Installing required packages..."
    
    # Detect OS and install packages
    if command -v apt-get &> /dev/null; then
        # Debian/Ubuntu
        apt-get update
        apt-get install -y nginx certbot python3-certbot-nginx curl
    elif command -v yum &> /dev/null; then
        # CentOS/RHEL
        yum install -y nginx certbot python3-certbot-nginx curl
    elif command -v dnf &> /dev/null; then
        # Fedora
        dnf install -y nginx certbot python3-certbot-nginx curl
    else
        print_error "Unsupported package manager"
        exit 1
    fi
    
    print_success "Packages installed"
}

# Create necessary directories
create_directories() {
    print_status "Creating necessary directories..."
    
    mkdir -p "$LOGS_DIR"
    mkdir -p "$BACKUPS_DIR"
    mkdir -p "$SSL_DIR"
    mkdir -p "$(dirname "$OLD_DB_PATH")"
    
    if [ ! -d "$MATERIA_DIR" ]; then
        mkdir -p "$MATERIA_DIR"
        chmod 755 "$MATERIA_DIR"
    fi
    
    # Set proper permissions
    chown -R 1000:1000 "$MATERIA_DIR" "$LOGS_DIR" "$BACKUPS_DIR"
    chmod -R 755 "$MATERIA_DIR" "$LOGS_DIR" "$BACKUPS_DIR"
    
    print_success "Directories created with proper permissions"
}

# Generate password hashes if they don't exist
generate_passwords() {
    print_status "Checking password hashes..."
    
    if [ ! -f "$MATERIA_DIR/snake.hash" ] || [ ! -f "$MATERIA_DIR/seva.hash" ]; then
        print_warning "Password hashes not found, generating..."
        
        if [ -f "scripts/generate-hashes.js" ]; then
            node scripts/generate-hashes.js
            print_success "Password hashes generated"
        else
            print_error "generate-hashes.js script not found"
            exit 1
        fi
    else
        print_success "Password hashes found"
    fi
}

# Setup SSL certificates
setup_ssl() {
    print_status "Setting up SSL certificates..."
    
    # Check if certificates already exist
    if [ -f "$SSL_DIR/cert.pem" ] && [ -f "$SSL_DIR/key.pem" ]; then
        print_success "SSL certificates already exist"
        return 0
    fi
    
    # Prompt for domain name
    read -p "Enter your domain name (e.g., materia.example.com): " DOMAIN_NAME
    
    if [ -z "$DOMAIN_NAME" ]; then
        print_warning "No domain provided, using self-signed certificate"
        generate_self_signed_cert
    else
        print_status "Obtaining SSL certificate for $DOMAIN_NAME..."
        obtain_letsencrypt_cert "$DOMAIN_NAME"
    fi
}

# Generate self-signed certificate
generate_self_signed_cert() {
    print_status "Generating self-signed SSL certificate..."
    
    openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
        -keyout "$SSL_DIR/key.pem" \
        -out "$SSL_DIR/cert.pem" \
        -subj "/C=US/ST=State/L=City/O=Organization/CN=localhost"
    
    print_success "Self-signed certificate generated"
}

# Obtain Let's Encrypt certificate
obtain_letsencrypt_cert() {
    local domain="$1"
    
    print_status "Obtaining Let's Encrypt certificate for $domain..."
    
    # Stop nginx temporarily
    systemctl stop nginx 2>/dev/null || true
    
    # Create temporary nginx config for certbot
    cat > /etc/nginx/sites-available/materia-temp << EOF
server {
    listen 80;
    server_name $domain;
    
    location /.well-known/acme-challenge/ {
        root /var/www/html;
    }
    
    location / {
        return 301 https://\$server_name\$request_uri;
    }
}
EOF
    
    # Enable temporary site
    ln -sf /etc/nginx/sites-available/materia-temp /etc/nginx/sites-enabled/
    systemctl start nginx
    
    # Obtain certificate
    certbot certonly --webroot -w /var/www/html -d "$domain" --non-interactive --agree-tos --email admin@$domain
    
    # Copy certificates to our directory
    cp /etc/letsencrypt/live/$domain/fullchain.pem "$SSL_DIR/cert.pem"
    cp /etc/letsencrypt/live/$domain/privkey.pem "$SSL_DIR/key.pem"
    
    # Clean up temporary config
    rm /etc/nginx/sites-enabled/materia-temp
    systemctl stop nginx
    
    print_success "Let's Encrypt certificate obtained"
}

# Setup Nginx reverse proxy
setup_nginx() {
    print_status "Setting up Nginx reverse proxy..."
    
    # Create nginx configuration
    cat > /etc/nginx/sites-available/materia << EOF
# HTTP to HTTPS redirect
server {
    listen 80;
    server_name _;
    return 301 https://\$server_name\$request_uri;
}

# HTTPS server
server {
    listen 443 ssl http2;
    server_name _;
    
    # SSL configuration
    ssl_certificate $SSL_DIR/cert.pem;
    ssl_certificate_key $SSL_DIR/key.pem;
    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_ciphers ECDHE-RSA-AES256-GCM-SHA512:DHE-RSA-AES256-GCM-SHA512:ECDHE-RSA-AES256-GCM-SHA384:DHE-RSA-AES256-GCM-SHA384;
    ssl_prefer_server_ciphers off;
    ssl_session_cache shared:SSL:10m;
    ssl_session_timeout 10m;
    
    # Security headers
    add_header Strict-Transport-Security "max-age=31536000; includeSubDomains" always;
    add_header X-Frame-Options DENY always;
    add_header X-Content-Type-Options nosniff always;
    add_header X-XSS-Protection "1; mode=block" always;
    
    # Proxy to Docker container
    location / {
        proxy_pass http://127.0.0.1:$HTTP_PORT;
        proxy_set_header Host \$host;
        proxy_set_header X-Real-IP \$remote_addr;
        proxy_set_header X-Forwarded-For \$proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto \$scheme;
        proxy_connect_timeout 30s;
        proxy_send_timeout 30s;
        proxy_read_timeout 30s;
    }
}
EOF
    
    # Enable site
    ln -sf /etc/nginx/sites-available/materia /etc/nginx/sites-enabled/
    
    # Remove default site
    rm -f /etc/nginx/sites-enabled/default
    
    # Test configuration
    nginx -t
    
    # Start nginx
    systemctl enable nginx
    systemctl start nginx
    
    print_success "Nginx configured and started"
}

# Create production docker-compose override
create_production_compose() {
    print_status "Creating production Docker Compose configuration..."
    
    cat > docker-compose.prod.yml << EOF
version: '3.8'

services:
  materia-v5:
    build:
      context: .
      dockerfile: Dockerfile
    container_name: materia-v5-production
    ports:
      - "127.0.0.1:$HTTP_PORT:$HTTP_PORT"
    environment:
      - NODE_ENV=production
      - PORT=$HTTP_PORT
      - JWT_SECRET=${JWT_SECRET:-$(openssl rand -base64 32)}
      - OLD_DB_PATH=$OLD_DB_PATH
    volumes:
      - $LOGS_DIR:/app/logs
      - $BACKUPS_DIR:/app/backups
      - $MATERIA_DIR:/materia
      - $OLD_DB_PATH:/app/legacy/database.db:ro
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "node", "-e", "require('http').get('http://localhost:$HTTP_PORT/health', (res) => { process.exit(res.statusCode === 200 ? 0 : 1) })"]
      interval: 30s
      timeout: 10s
      retries: 3
      start_period: 40s

volumes:
  materia_data:
    driver: local
EOF
    
    print_success "Production Docker Compose configuration created"
}

# Deploy the application
deploy_application() {
    print_status "Deploying application..."
    
    # Stop existing containers
    docker-compose -f docker-compose.prod.yml down 2>/dev/null || true
    
    # Build and start
    docker-compose -f docker-compose.prod.yml up -d --build
    
    print_success "Application deployed"
}

# Wait for application to be ready
wait_for_ready() {
    print_status "Waiting for application to be ready..."
    
    local max_attempts=30
    local attempt=1
    
    while [ $attempt -le $max_attempts ]; do
        if curl -s http://localhost:$HTTP_PORT/health > /dev/null 2>&1; then
            print_success "Application is ready!"
            return 0
        fi
        
        print_status "Attempt $attempt/$max_attempts - waiting..."
        sleep 2
        attempt=$((attempt + 1))
    done
    
    print_error "Application failed to start within expected time"
    return 1
}

# Setup automatic SSL renewal
setup_ssl_renewal() {
    print_status "Setting up automatic SSL renewal..."
    
    # Create renewal script
    cat > /materia/scripts/renew-ssl.sh << 'EOF'
#!/bin/bash
certbot renew --quiet
cp /etc/letsencrypt/live/*/fullchain.pem /materia/ssl/cert.pem
cp /etc/letsencrypt/live/*/privkey.pem /materia/ssl/key.pem
systemctl reload nginx
EOF
    
    chmod +x /materia/scripts/renew-ssl.sh
    
    # Add to crontab
    (crontab -l 2>/dev/null; echo "0 12 * * * /materia/scripts/renew-ssl.sh") | crontab -
    
    print_success "SSL renewal configured"
}

# Display deployment information
show_deployment_info() {
    echo ""
    echo "🎉 Materia V5 Production Deployment Complete!"
    echo ""
    echo "📋 Deployment Information:"
    echo "   Application: $APP_NAME"
    echo "   HTTP Port: $HTTP_PORT (internal)"
    echo "   HTTPS Port: $HTTPS_PORT (external)"
    echo "   Logs Directory: $LOGS_DIR"
    echo "   SSL Directory: $SSL_DIR"
    echo "   Old DB Path: $OLD_DB_PATH"
    echo ""
    echo "🌐 Access URLs:"
    echo "   Snake's Interface: https://your-domain.com/snake"
    echo "   Seva's Interface: https://your-domain.com/seva"
    echo "   API: https://your-domain.com/api"
    echo "   Health Check: https://your-domain.com/health"
    echo ""
    echo "📝 Useful Commands:"
    echo "   View logs: docker-compose -f docker-compose.prod.yml logs -f"
    echo "   Stop: docker-compose -f docker-compose.prod.yml down"
    echo "   Restart: docker-compose -f docker-compose.prod.yml restart"
    echo "   SSL renewal: /opt/materia/scripts/renew-ssl.sh"
    echo ""
    echo "🔒 Security Notes:"
    echo "   - HTTPS is enabled with SSL certificates"
    echo "   - Automatic SSL renewal is configured"
    echo "   - Security headers are enabled"
    echo "   - Old database is mounted read-only"
    echo ""
}

# Main deployment process
main() {
    echo "=========================================="
    echo "   Materia V5 Production Deployment Script"
    echo "=========================================="
    echo ""
    
    check_root
    check_prerequisites
    install_packages
    create_directories
    generate_passwords
    setup_ssl
    setup_nginx
    create_production_compose
    deploy_application
    wait_for_ready
    setup_ssl_renewal
    show_deployment_info
}

# Run main function
main "$@"

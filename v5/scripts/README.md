# Materia V5 Deployment Scripts

This directory contains deployment scripts for the Materia V5 system.

## Scripts Overview

### 1. `deploy-test.sh` - Test Deployment (Linux/Mac)
A simple deployment script for testing and development environments.

**Features:**
- Basic Docker deployment
- Local development setup
- Automatic directory creation
- Password hash generation
- Health check verification

**Usage:**
```bash
# Make executable (Linux/Mac)
chmod +x scripts/deploy-test.sh

# Run the script
./scripts/deploy-test.sh
```

### 2. `deploy-test.ps1` - Test Deployment (Windows PowerShell)
A Windows PowerShell version of the test deployment script.

**Features:**
- Windows-specific Docker checks
- PowerShell-native error handling
- Colored output for better UX
- Automatic directory creation
- Password hash generation
- Health check verification

**Usage:**
```powershell
# Run the script (PowerShell)
.\scripts\deploy-test.ps1

# Run with execution policy bypass if needed
PowerShell -ExecutionPolicy Bypass -File .\scripts\deploy-test.ps1
```

**Access URLs:**
- Snake: http://localhost:3000/snake
- Seva: http://localhost:3000/seva
- API: http://localhost:3000/api
- Health: http://localhost:3000/health

### 3. `deploy-production.sh` - Production Deployment
A comprehensive production deployment script with HTTPS and security features.

**Features:**
- HTTPS with SSL certificates (Let's Encrypt or self-signed)
- Nginx reverse proxy
- Automatic SSL renewal
- Production-grade security headers
- Legacy database migration support
- Proper file permissions
- System service management

**Usage:**
```bash
# Make executable (Linux/Mac)
chmod +x scripts/deploy-production.sh

# Run as root (required for production)
sudo ./scripts/deploy-production.sh
```

**Prerequisites:**
- Root access (sudo)
- Domain name (for Let's Encrypt certificates)
- Docker and Docker Compose installed
- Internet access (for Let's Encrypt)

## Configuration

### Test Environment
- **Port**: 3000
- **Data Directory**: `./data`
- **Logs Directory**: `./logs`
- **Backups Directory**: `./backups`
- **Materia Directory**: `/materia`

### Production Environment
- **HTTP Port**: 80 (internal)
- **HTTPS Port**: 443 (external)
- **Data Directory**: `/materia/data`
- **Logs Directory**: `/materia/logs`
- **Backups Directory**: `/materia/backups`
- **SSL Directory**: `/materia/ssl`
- **Old DB Path**: `/materia/legacy/database.db`
- **New DB Path**: `/materia/data/materia.db`

## Database Migration

The production script supports migration from the legacy system:

1. **Old Database**: Mounted read-only at `/app/legacy/database.db`
2. **New Database**: SQLite database at `/app/data/materia.db`
3. **Migration Path**: Backend can read from old DB, writes to new DB

## SSL Certificate Options

### Let's Encrypt (Recommended)
- Free, trusted certificates
- Automatic renewal
- Requires domain name
- Requires port 80/443 access

### Self-Signed
- For testing or internal use
- Browser warnings
- No domain required
- Generated automatically

## Security Features

### Production Security
- HTTPS enforcement
- Security headers (HSTS, X-Frame-Options, etc.)
- SSL/TLS 1.2+ only
- Automatic certificate renewal
- Read-only legacy database mount
- Proper file permissions

### Authentication
- JWT tokens
- bcrypt password hashing
- Password hashes stored in `/materia` directory

## Troubleshooting

### Common Issues

1. **Permission Denied**
   ```bash
   # Make scripts executable
   chmod +x scripts/*.sh
   ```

2. **Port Already in Use**
   ```bash
   # Check what's using the port
   sudo netstat -tulpn | grep :3000
   
   # Stop conflicting services
   sudo systemctl stop nginx  # if needed
   ```

3. **SSL Certificate Issues**
   ```bash
   # Check certificate status
   sudo certbot certificates
   
   # Renew manually
   sudo /opt/materia/scripts/renew-ssl.sh
   ```

4. **Docker Issues**
   ```bash
   # Check container status
   docker-compose ps
   
   # View logs
   docker-compose logs -f
   ```

### Log Locations

- **Application Logs**: `/materia/logs/` (production) or `./logs/` (test)
- **Nginx Logs**: `/var/log/nginx/`
- **Docker Logs**: `docker-compose logs -f`

### Health Checks

```bash
# Test application health
curl http://localhost:3000/health

# Test HTTPS (production)
curl https://your-domain.com/health
```

## Maintenance

### Regular Tasks

1. **SSL Renewal**: Automatic via cron job
2. **Backups**: Manual or automated via scripts
3. **Updates**: Pull new code and redeploy
4. **Log Rotation**: Configure logrotate for nginx

### Update Process

```bash
# 1. Pull latest code
git pull

# 2. Rebuild and restart
docker-compose -f docker-compose.prod.yml down
docker-compose -f docker-compose.prod.yml up -d --build
```

## Windows Usage

On Windows, you have several options:

### Option 1: PowerShell Script (Recommended)
```powershell
# Run the Windows PowerShell script
.\scripts\deploy-test.ps1

# If you get execution policy errors
PowerShell -ExecutionPolicy Bypass -File .\scripts\deploy-test.ps1
```

### Option 2: WSL (Windows Subsystem for Linux)
```bash
# In WSL
chmod +x scripts/deploy-test.sh
./scripts/deploy-test.sh
```

### Option 3: Git Bash
```bash
# In Git Bash
chmod +x scripts/deploy-test.sh
./scripts/deploy-test.sh
```

## Support

For issues or questions:
1. Check the logs: `docker-compose logs -f`
2. Verify configuration files
3. Test individual components
4. Review the main README.md for additional information


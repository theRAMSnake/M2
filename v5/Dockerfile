# Multi-stage build for the complete application
FROM node:20-alpine AS builder

# Install Python and build dependencies for native modules
RUN apk add --no-cache python3 make g++

# Set working directory
WORKDIR /app

# Copy package files
COPY package*.json ./
COPY webapp/package*.json ./webapp/

# Install dependencies
RUN npm ci
RUN cd webapp && npm ci

# Copy source code
COPY . .

# Build the backend
RUN npm run build:backend

# Build the webapp for production
RUN cd webapp && npm run build

# Production stage
FROM node:20-alpine AS runner

# Install Python for native modules (required at runtime)
RUN apk add --no-cache python3

# Set working directory
WORKDIR /app

# Copy package files and install production dependencies
COPY package*.json ./
RUN npm ci --only=production

# Copy built backend
COPY --from=builder /app/dist ./dist

# Copy built webapp static files (Next.js static export)
COPY --from=builder /app/webapp/out ./webapp/out

# Create necessary directories
RUN mkdir -p data logs backups /materia

# Create non-root user for security
RUN addgroup -g 1001 -S nodejs
RUN adduser -S nextjs -u 1001

# Change ownership of the app directory
RUN chown -R nextjs:nodejs /app

# Create a startup script to handle permissions
RUN echo '#!/bin/sh' > /app/start.sh && \
    echo 'mkdir -p /materia' >> /app/start.sh && \
    echo 'chown -R nextjs:nodejs /materia' >> /app/start.sh && \
    echo 'exec su-exec nextjs "$@"' >> /app/start.sh && \
    chmod +x /app/start.sh

# Install su-exec for better user switching
RUN apk add --no-cache su-exec

# Expose port
EXPOSE 3000

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
  CMD node -e "require('http').get('http://localhost:3000/health', (res) => { process.exit(res.statusCode === 200 ? 0 : 1) })"

# Start the application with proper permissions
CMD ["/app/start.sh", "npm", "start"]

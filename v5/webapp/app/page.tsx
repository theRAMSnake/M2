'use client';

import { useState, useEffect } from 'react';
import { Box, CircularProgress, Typography } from '@mui/material';
import { LoginForm } from './components/LoginForm';
import { Dashboard } from './components/Dashboard';

interface User {
  id: string;
  name: string;
}

export default function Home() {
  const [user, setUser] = useState<User | null>(null);
  const [loading, setLoading] = useState(true);
  const [currentUser, setCurrentUser] = useState<string>('');

  useEffect(() => {
    // Detect user from URL path
    const path = window.location.pathname;
    if (path.startsWith('/snake')) {
      setCurrentUser('snake');
    } else if (path.startsWith('/seva')) {
      setCurrentUser('seva');
    } else {
      setCurrentUser('snake'); // Default to snake
    }

    // Check if user is already logged in
    const token = localStorage.getItem('materia-token');
    if (token) {
      verifyToken(token);
    } else {
      setLoading(false);
    }
  }, []);

  const verifyToken = async (token: string) => {
    try {
      const response = await fetch('/api/auth/verify', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });
      
      if (response.ok) {
        const data = await response.json();
        setUser(data.user);
      } else {
        localStorage.removeItem('materia-token');
      }
    } catch (error) {
      console.error('Token verification failed:', error);
      localStorage.removeItem('materia-token');
    } finally {
      setLoading(false);
    }
  };

  const handleLogin = (userData: User, token: string) => {
    setUser(userData);
    localStorage.setItem('materia-token', token);
  };

  const handleLogout = () => {
    setUser(null);
    localStorage.removeItem('materia-token');
  };

  if (loading) {
    return (
      <Box
        sx={{
          minHeight: '100vh',
          display: 'flex',
          flexDirection: 'column',
          alignItems: 'center',
          justifyContent: 'center',
          bgcolor: 'background.default',
        }}
      >
        <CircularProgress size={60} />
        <Typography variant="body1" sx={{ mt: 2, color: 'text.secondary' }}>
          Loading...
        </Typography>
      </Box>
    );
  }

  return (
    <Box sx={{ minHeight: '100vh' }}>
      {user ? (
        <Dashboard user={user} onLogout={handleLogout} currentUser={currentUser} />
      ) : (
        <LoginForm onLogin={handleLogin} currentUser={currentUser} />
      )}
    </Box>
  );
}

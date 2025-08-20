'use client';

import { useState } from 'react';
import {
  Box,
  Card,
  CardContent,
  TextField,
  Button,
  Typography,
  Alert,
  Container,
} from '@mui/material';
import { LockOutlined } from '@mui/icons-material';

interface User {
  id: string;
  name: string;
}

interface LoginFormProps {
  onLogin: (user: User, token: string) => void;
  currentUser: string;
}

export function LoginForm({ onLogin, currentUser }: LoginFormProps) {
  const [password, setPassword] = useState('');
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState('');

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setLoading(true);
    setError('');

    try {
      const response = await fetch('/api/auth/login', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ username: currentUser, password }),
      });

      const data = await response.json();

      if (response.ok) {
        onLogin(data.user, data.token);
      } else {
        setError(data.message || 'Login failed');
      }
    } catch (error) {
      setError('Network error. Please try again.');
    } finally {
      setLoading(false);
    }
  };

  const getUserDisplayName = () => {
    return currentUser === 'seva' ? 'Seva' : 'Snake';
  };

  return (
    <Box
      sx={{
        minHeight: '100vh',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        p: 2,
        bgcolor: 'background.default',
      }}
    >
      <Container maxWidth="sm">
        <Card
          sx={{
            p: 4,
            textAlign: 'center',
            bgcolor: 'background.paper',
          }}
        >
          <CardContent>
            <Box sx={{ mb: 4 }}>
              <LockOutlined sx={{ fontSize: 48, color: 'primary.main', mb: 2 }} />
              <Typography variant="h4" component="h1" gutterBottom>
                Welcome back, {getUserDisplayName()}!
              </Typography>
            </Box>

            <Box component="form" onSubmit={handleSubmit} sx={{ mt: 3 }}>
              <TextField
                fullWidth
                type="password"
                label="Password"
                value={password}
                onChange={(e) => setPassword(e.target.value)}
                required
                margin="normal"
                variant="outlined"
                disabled={loading}
                autoFocus
              />

              {error && (
                <Alert severity="error" sx={{ mt: 2 }}>
                  {error}
                </Alert>
              )}

              <Button
                type="submit"
                fullWidth
                variant="contained"
                size="large"
                disabled={loading}
                sx={{ mt: 3, mb: 2 }}
              >
                {loading ? 'Signing in...' : 'Sign in'}
              </Button>
            </Box>
          </CardContent>
        </Card>
      </Container>
    </Box>
  );
}

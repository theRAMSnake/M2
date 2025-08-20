'use client';

import {
  Container,
  Paper,
  Typography,
} from '@mui/material';

interface User {
  id: string;
  name: string;
}

interface DashboardAppProps {
  user: User;
}

export function DashboardApp({ user }: DashboardAppProps) {
  return (
    <Container maxWidth="lg" sx={{ py: 4 }}>
      <Paper
        sx={{
          p: 4,
          textAlign: 'center',
          bgcolor: 'background.paper',
        }}
      >
        <Typography variant="h4" component="h1" gutterBottom>
          Welcome back, {user.name}
        </Typography>
        <Typography variant="body1" color="text.secondary">
          Your personal Materia interface is ready.
        </Typography>
      </Paper>
    </Container>
  );
}

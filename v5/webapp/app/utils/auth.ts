export function getCurrentUser(): string {
  const path = window.location.pathname;
  if (path.startsWith('/snake')) {
    return 'snake';
  } else if (path.startsWith('/seva')) {
    return 'seva';
  }
  return 'snake'; // Default
}

export function getAuthToken(): string | null {
  const currentUser = getCurrentUser();
  return localStorage.getItem(`materia-token-${currentUser}`);
}

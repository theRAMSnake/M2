// Frontend ID generator using native crypto.randomUUID()
export const generateId = (existingId?: string): string => {
  if (existingId) return existingId;
  
  // Use native crypto.randomUUID() for proper UUID v4 generation
  return crypto.randomUUID();
};

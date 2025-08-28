export interface ColorPreset {
  value: string;
  label: string;
  color: string;
}

export const COLOR_PRESETS: ColorPreset[] = [
  { value: 'red', label: 'Red', color: '#f44336' },
  { value: 'pink', label: 'Pink', color: '#e91e63' },
  { value: 'purple', label: 'Purple', color: '#9c27b0' },
  { value: 'blue', label: 'Blue', color: '#2196f3' },
  { value: 'green', label: 'Green', color: '#4caf50' },
  { value: 'yellow', label: 'Yellow', color: '#ffeb3b' },
  { value: 'orange', label: 'Orange', color: '#ff9800' },
  { value: 'brown', label: 'Brown', color: '#795548' },
  { value: 'grey', label: 'Grey', color: '#9e9e9e' },
];

export const getColorValue = (colorName: string): string => {
  const preset = COLOR_PRESETS.find(c => c.value === colorName);
  return preset ? preset.color : colorName;
};

export const getDefaultColor = (): string => {
  return COLOR_PRESETS[3].value; // Blue as default
};

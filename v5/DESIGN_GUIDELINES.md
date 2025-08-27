# Materia V5 Design Guidelines

## General Principles

1. **Mobile-First Design**: All components must be mobile-friendly and responsive
2. **Consistent UI**: Use Material-UI components and follow established patterns
3. **Performance**: Optimize for fast loading and smooth interactions

## App Development Guidelines

### App Structure
- Each app should be a separate component in `v5/webapp/app/components/apps/`
- Export the app from `v5/webapp/app/components/apps/index.ts`
- Register the app in `v5/webapp/app/components/Dashboard.tsx`

### Data Storage
- Use the document API for data persistence
- Store data in `/shared/` for shared apps, `/user/` for user-specific data
- Follow the established path structure (e.g., `/shared/stickies/`)

### User Access
- Configure app access in the `APP_CONFIG` object in Dashboard.tsx
- Available users: `snake`, `seva`
- Apps can be available to specific users or all users

## Dependency Management

### Before Installing Dependencies
**ALWAYS explain why dependencies are needed before installing them:**

1. **Identify the requirement**: What feature needs the dependency?
2. **Research alternatives**: Are there built-in solutions or lighter alternatives?
3. **Explain the choice**: Why this specific package over others?
4. **Document the decision**: Add to this guidelines document

### Example Dependency Explanation
```
For the stickies app, I need to install:
- @tiptap/react - Modern, extensible rich text editor for React
- @tiptap/pm - Required ProseMirror dependency for TipTap
- @tiptap/starter-kit - Essential formatting extensions (bold, italic, etc.)
- @tiptap/extension-placeholder - Shows placeholder text when empty

These are needed because the user requested "rich edit modifiable" content for stickies.
```

### Dependency Guidelines
- Prefer lightweight, well-maintained packages
- Avoid large dependencies when simple solutions exist
- Document why each dependency is necessary
- Consider bundle size impact

## Version Management

### Incrementing Versions
- Increment version in both `v5/package.json` and `v5/webapp/package.json`
- Follow semantic versioning (major.minor.patch)
- Document changes in commit messages

## Code Quality

### TypeScript
- Use strict TypeScript configuration
- Define proper interfaces for all data structures
- Avoid `any` types when possible

### Error Handling
- Implement proper error boundaries
- Show user-friendly error messages
- Log errors for debugging
- No silent errors, if something cannot be done - bo loud

### Testing
- Write tests for critical functionality
- Test both success and error scenarios
- Ensure mobile compatibility

## UI/UX Guidelines

### Layout
- Use Material-UI Grid system for responsive layouts
- Maintain consistent spacing and padding
- Ensure touch targets are at least 44px

### Colors and Theming
- Use Material-UI theme system
- Support both light and dark modes
- Ensure sufficient color contrast

### Interactions
- Provide immediate feedback for user actions
- Use loading states for async operations
- Implement proper form validation

## Security

### Authentication
- Always verify JWT tokens
- Use proper authorization checks
- Sanitize user inputs

### Data Protection
- Validate all API inputs
- Use HTTPS in production
- Implement proper CORS policies

## Performance

### Optimization
- Lazy load components when possible
- Optimize images and assets
- Minimize bundle size

### Caching
- Implement appropriate caching strategies
- Use React.memo for expensive components
- Cache API responses when appropriate

## Documentation

### Code Comments
- Document complex business logic
- Explain non-obvious design decisions
- Keep comments up to date

### User Documentation
- Provide clear feature descriptions
- Include usage examples
- Document any configuration requirements

# Contributing to AI Guitar Plugin

Thank you for your interest in contributing! This document provides guidelines and instructions for contributing to the project.

## Getting Started

1. Fork the repository
2. Clone your fork: `git clone https://github.com/your-username/Last-Plugin.git`
3. Create a branch: `git checkout -b feature/your-feature-name`
4. Make your changes
5. Test thoroughly
6. Commit your changes: `git commit -m "Add: description of changes"`
7. Push to your fork: `git push origin feature/your-feature-name`
8. Open a Pull Request

## Development Setup

### Prerequisites
- macOS (for plugin development)
- Xcode (latest version)
- CMake 3.22+
- Node.js 18+ (for AI server)

### Building the Plugin

```bash
mkdir build
cd build
cmake .. -G Xcode
open AIGuitarPlugin.xcodeproj
```

Build for Universal Binary (arm64 + x86_64) in Xcode.

### Running the AI Server

```bash
cd preset-server
npm install
cp env.example .env
# Add your OPENAI_API_KEY to .env
npm start
```

## Code Style

### C++/JUCE
- Follow JUCE coding conventions
- Use camelCase for variables and functions
- Use PascalCase for classes
- Keep functions focused and small
- Add comments for complex DSP algorithms
- Use `const` where possible
- Prefer `auto` for type inference when clear

### JavaScript/Node.js
- Use ES6+ features
- Follow async/await patterns
- Add error handling
- Validate inputs

## Commit Messages

Use clear, descriptive commit messages:
- `Add: feature description`
- `Fix: bug description`
- `Update: component/module description`
- `Refactor: what was refactored`
- `Docs: documentation changes`

## Pull Request Guidelines

1. **Keep PRs focused**: One feature or bug fix per PR
2. **Add tests**: If applicable, add tests for new features
3. **Update documentation**: Update README or relevant docs if needed
4. **Check build**: Ensure the project builds successfully
5. **Test thoroughly**: Test on different DAWs and sample rates if possible

## Areas for Contribution

### DSP
- New effect algorithms
- Performance optimizations
- Audio quality improvements
- Oversampling improvements

### UI/UX
- UI component improvements
- Visual feedback enhancements
- Accessibility improvements
- Theme customization

### AI Integration
- Prompt engineering improvements
- Model optimization
- Error handling
- Caching strategies

### Testing
- Unit tests
- Integration tests
- Performance benchmarks
- Cross-platform testing

### Documentation
- Code comments
- API documentation
- User guides
- Tutorial videos

## Reporting Issues

Before reporting an issue:
1. Check existing issues to avoid duplicates
2. Try to reproduce the issue consistently
3. Gather system information (OS, DAW, plugin format, etc.)
4. Include error messages or logs

Use the bug report template when opening issues.

## Feature Requests

When requesting features:
1. Describe the use case clearly
2. Explain why it would be valuable
3. Suggest potential implementation if possible
4. Consider backward compatibility

## Questions?

- Open a discussion on GitHub
- Check existing issues and discussions
- Review the documentation

Thank you for contributing! 🎸

# Security Policy

## Supported Versions

We currently support the following versions with security updates:

| Version | Supported          |
| ------- | ------------------ |
| 1.x.x   | :white_check_mark: |

## Reporting a Vulnerability

If you discover a security vulnerability, please **do not** open a public issue. Instead, please email security concerns to the maintainers or create a private security advisory.

### What to Report

Please report:
- API key exposure risks
- Input validation issues
- Buffer overflows
- Memory leaks that could lead to crashes
- Network security issues
- Authentication/authorization flaws

### Response Time

We aim to:
- Acknowledge receipt within 48 hours
- Provide an initial assessment within 7 days
- Keep you informed of progress and resolution timeline

## Security Best Practices

When using this plugin:

1. **API Keys**: Never commit your OpenAI API key to version control
2. **Server**: Run the AI server locally or on a secure server you control
3. **Network**: Use HTTPS in production environments
4. **Updates**: Keep dependencies updated
5. **Environment**: Use environment variables for sensitive configuration

## Known Security Considerations

- The plugin communicates with a local Node.js server for AI generation
- API keys are stored server-side in environment variables
- No authentication is required for local server endpoints (by design for local use)
- For production deployments, consider adding authentication middleware

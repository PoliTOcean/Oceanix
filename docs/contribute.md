# Contributing to the Project

Thank you for your interest in contributing to our C++ project! We follow the Google C++ Style Guide to maintain code quality and consistency. Please take a moment to review these guidelines before contributing.

## Debug standard

Every class for debug must use the `print_log()` function

```
[CLASS_NAME][MSG_TYPE] msg...
CLASS_NAME must be 7 chars len
MSG_TYPE must be 5 chars len 
MSG_TYPE = [DEBUG | INFO_ | WARN_ | ERROR]
```

verbose argument enable DEBUG messages INFO, WARN, ERROR are enabled by default
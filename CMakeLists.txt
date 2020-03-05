cmake_minimum_required(VERSION 3.0)

project(GIT_FILTER_DEMO
    VERSION 0.1.0
    LANGUAGES C
    )

set(PROJECT_NAME_PRETTY "git-filter-demo")

set(PROJECT_DESCRIPTION
"Demonstrate use of Git filters to auto-format code to your preferred \
indentation style during checkout and staging."
)

set(PROJECT_HOMEPAGE_URL "https://github.com/shoogle/git-filter-demo")

add_executable(main main.c)

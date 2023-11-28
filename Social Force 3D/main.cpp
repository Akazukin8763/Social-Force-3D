#define GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Application.h"

int main(int argc, char** argv) {
    srand(time(NULL));

    Application application(800, 800);
    application.Execute();
}
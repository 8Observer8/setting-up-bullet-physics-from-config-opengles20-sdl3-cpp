#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#ifdef __EMSCRIPTEN__
#include <SDL3/SDL_opengles2.h>
#else
#include <glad/glad.h>
#endif // __EMSCRIPTEN__

#include <iostream>

#include <btBulletDynamicsCommon.h>

btBroadphaseInterface *overlappingPairCache;
btCollisionDispatcher *dispatcher;
btCollisionConfiguration *collisionConfiguration;
btConstraintSolver *solver;
btDynamicsWorld *world;

struct AppContext
{
    SDL_Window *window;
    SDL_GLContext glContext;
    SDL_AppResult appQuit = SDL_APP_CONTINUE;
};

SDL_AppResult SDL_Fail()
{
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    // Init the SDL3 library
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        return SDL_Fail();
    }

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // Enable MULTISAMPLE
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2); // can be 2, 4, 8 or 16

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Setting up Bullet Physics from the config", 380, 380,
        SDL_WINDOW_OPENGL); // | SDL_WINDOW_RESIZABLE
    if (!window)
    {
        return SDL_Fail();
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        return SDL_Fail();
    }

    SDL_GL_SetSwapInterval(1);

#ifdef __WIN32__
    if (!gladLoadGL())
    {
        std::cout << "Failed to load OpenGL functions" << std::endl;
        return SDL_APP_FAILURE;
    }
#endif // __WIN32__

    glClearColor(0.2f, 0.5f, 0.3f, 1.f);

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    // Output:
    // OpenGL version: 3.1.0 - Build 9.17.10.4459
    // GLSL version: 1.40 - Intel Build 9.17.10.4459
    // Vendor: Intel

    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache,
        solver, collisionConfiguration);
    world->setGravity(btVector3(0, -9.8, 0));
    btVector3 gravity = world->getGravity();
    float gx = gravity.x();
    float gy = gravity.y();
    float gz = gravity.z();
    std::cout << "gravity = (" << gx << ", " << gy << ", " << gz << ")" << std::endl;

    SDL_ShowWindow(window);

    // Set up the application data
    AppContext *app = new AppContext();
    app->window = window;
    app->glContext = glContext;
    *appstate = app;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    auto *app = (AppContext *)appstate;

    switch (event->type)
    {
        case SDL_EVENT_QUIT:
        {
            app->appQuit = SDL_APP_SUCCESS;
            break;
        }
        default:
        {
            break;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    auto *app = (AppContext *)appstate;

    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(app->window);

    return app->appQuit;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    auto *app = (AppContext *)appstate;
    if (app)
    {
        SDL_GL_DestroyContext(app->glContext);
        SDL_DestroyWindow(app->window);
        delete app;
    }

    delete world;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;
    SDL_Quit();
}

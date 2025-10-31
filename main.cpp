#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "Quad.hpp"
#include "Texture.hpp"
#include "TextureShader.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>



int getTransformKey(GLFWwindow *openGLWindow, bool renderWithOpenGL)
{
    if (renderWithOpenGL && openGLWindow)
    {
        if (glfwGetKey(openGLWindow, GLFW_KEY_I) == GLFW_PRESS)
            return 'i';
        if (glfwGetKey(openGLWindow, GLFW_KEY_O) == GLFW_PRESS)
            return 'o';
        if (glfwGetKey(openGLWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
            return 2424832;
        if (glfwGetKey(openGLWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
            return 2555904;
        if (glfwGetKey(openGLWindow, GLFW_KEY_UP) == GLFW_PRESS)
            return 2490368;
        if (glfwGetKey(openGLWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
            return 2621440;
        return -1;
    }
    else
    {
        return cv::waitKeyEx(1);
    }
}

void renderFrameWithOpenGL(GLFWwindow *window, const cv::Mat &frame, Scene *scene, Camera *camera, Texture *&texture)
{
    if (glfwWindowShouldClose(window))
    {
        return;
    }

    // update Texture
    if (!frame.empty() && texture != nullptr)
    {
        cv::Mat flippedFrame;
        cv::flip(frame, flippedFrame, 0);
        texture->update(flippedFrame.data, flippedFrame.cols, flippedFrame.rows, true);
    }

    // render texture
    glClearColor(0.1f, 0.1f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (scene != nullptr && camera != nullptr)
    {
        scene->render(camera);
    }
    else
    {
        std::cout << "Scene null: " << (scene == nullptr)
                  << ", Camera null: " << (camera == nullptr)
                  << ", Texture null: " << (texture == nullptr) << std::endl;
    }

    // finalise frame
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

GLFWwindow *initializeOpenGLWindow(int width, int height)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(width, height, "OpenGL Video", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // Set the viewport size
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

int main()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    int desired_width = 640;
    int desired_height = 360;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, desired_width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, desired_height);
    // -------------------------------------------

    int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    std::cout << "Requested Resolution: " << desired_width << "x" << desired_height << std::endl;
    std::cout << "Actual Resolution set by camera: " << frame_width << "x" << frame_height << std::endl;

    cv::Mat frame;
    bool isGrayscale = false;
    bool isGaussian = false;
    bool isEdge = false;
    bool isPixelated = false;

    // opengl
    bool renderWithOpenGL = false;
    GLFWwindow *openGLWindow = nullptr;
    Scene *myScene = nullptr;
    Camera *renderingCamera = nullptr;
    TextureShader *textureShader = nullptr;
    Texture *videoTexture = nullptr;
    Quad *myQuad = nullptr;

    // for transformations
    float currentScale = 1.0f;
    float currentAngle = 0.0f;
    glm::vec2 currentTranslation(0.0f, 0.0f);
    bool transformation = true;

    glm::mat4 modelMatrixGPU = glm::mat4(1.0f);
    cv::Mat modelMatrixCPU = cv::Mat::eye(2, 3, CV_32F);

    int frameCount = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastTime = std::chrono::high_resolution_clock::now();
    bool resetFpsCounter = true;

    while (true)
    {
        cap.read(frame);
        if (frame.empty())
        {
            std::cerr << "Error: Captured empty frame." << std::endl;
            break;
        }

        int key = -1;

        if (renderWithOpenGL)
        {
            if (openGLWindow)
            {
                if (glfwGetKey(openGLWindow, GLFW_KEY_G) == GLFW_PRESS)
                    key = 'g';
                if (glfwGetKey(openGLWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                    key = 27;
            }
        }
        else
        {
            key = cv::waitKey(1);
        }

        if (key == 27)
        { // esc to exit
            break;
        }
        if (key == '1')
        {
            isGrayscale = !isGrayscale;
            resetFpsCounter = true;
        }
        if (key == '2')
        {
            isGaussian = !isGaussian;
            resetFpsCounter = true;
        }
        if (key == '3')
        {
            isEdge = !isEdge;
            resetFpsCounter = true;
        }
        if (key == '4')
        {
            isPixelated = !isPixelated;
            resetFpsCounter = true;
        }

        // Toggle rendering mode with g
        if (key == 'g')
        {
            renderWithOpenGL = !renderWithOpenGL;
            resetFpsCounter = true;

            if (renderWithOpenGL)
            { // Switch TO OpenGL
                std::cout << "Switching to OpenGL rendering" << std::endl;
                cv::destroyAllWindows(); // Close all OpenCV windows
                openGLWindow = initializeOpenGLWindow(frame_width, frame_height);
                if (openGLWindow == nullptr)
                {
                    std::cerr << "Failed - returning to OpenCV mode." << std::endl;
                    renderWithOpenGL = false;
                }
                else
                {
                    GLuint VertexArrayID;
                    glGenVertexArrays(1, &VertexArrayID);
                    glBindVertexArray(VertexArrayID);
                    float aspectRatio = static_cast<float>(frame_width) / static_cast<float>(frame_height);
                    if (isGrayscale)
                    {
                        textureShader = new TextureShader("TextureShader.vert", "TextureShaderBW.frag");
                    }
                    else if (isPixelated)
                    {
                        textureShader = new TextureShader("TextureShader.vert", "TextureShaderPixelate.frag");
                    }
                    else
                    {
                        textureShader = new TextureShader("TextureShader.vert", "TextureShader.frag");
                    }
                    myScene = new Scene();
                    renderingCamera = new Camera(aspectRatio);

                    renderingCamera->setPosition(glm::vec3(0.0f, 0.0f, 1.5f));
                    myQuad = new Quad(aspectRatio);
                    cv::Mat flippedFrame;
                    cv::flip(frame, flippedFrame, 0);
                    videoTexture = new Texture(flippedFrame.data, flippedFrame.cols, flippedFrame.rows, true);
                    myQuad->setShader(textureShader);
                    textureShader->setTexture(videoTexture);
                    myScene->addObject(myQuad);
                    std::cout << "OpenGL init complete" << std::endl;
                }
            }
            else
            { // Switch BACK FROM OpenGL
                std::cout << "Switching to OpenCV rendering" << std::endl;
                if (openGLWindow != nullptr)
                {
                    delete myScene;
                    myScene = nullptr;
                    delete renderingCamera;
                    renderingCamera = nullptr;
                    glfwDestroyWindow(openGLWindow);
                    glfwTerminate();
                    openGLWindow = nullptr;
                }
            }
        }
        int transformKey = getTransformKey(openGLWindow, renderWithOpenGL);

        if (transformKey == 'i')
        {
            currentScale += 0.1f;
            transformation = true;
        }
        if (transformKey == 'o')
        {
            currentScale -= 0.1f;
            transformation = true;
        }
        if (transformKey == 2424832)
        {
            currentAngle -= 5.0f;
            transformation = true;
        }
        if (transformKey == 2555904)
        {
            currentAngle += 5.0f;
            transformation = true;
        }
        if (transformKey == 2490368)
        {
            currentTranslation.y += 0.1f;
            transformation = true;
        }
        if (transformKey == 2621440)
        {
            currentTranslation.y -= 0.1f;
            transformation = true;
        }

        if (transformation)
        {
            glm::mat4 transMat = glm::translate(glm::mat4(1.0f), glm::vec3(currentTranslation, 0.0f));
            glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(currentAngle), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(currentScale, currentScale, 1.0f));
            modelMatrixGPU = transMat * rotMat * scaleMat;

            cv::Point2f center(frame.cols / 2.0f, frame.rows / 2.0f);
            modelMatrixCPU = cv::getRotationMatrix2D(center, -currentAngle, currentScale);
            modelMatrixCPU.at<double>(1, 2) -= currentTranslation.y * frame.rows;

            transformation = false;
        }

        if (renderWithOpenGL)
        {
            if (openGLWindow && !glfwWindowShouldClose(openGLWindow))
            {
                myQuad->addTransform(modelMatrixGPU);
                renderFrameWithOpenGL(openGLWindow, frame, myScene, renderingCamera, videoTexture);
            }
            else
            {
                // Window was closed by user, trigger cleanup
                renderWithOpenGL = false;
                if (openGLWindow != nullptr)
                {
                    delete myScene;
                    myScene = nullptr;
                    delete renderingCamera;
                    renderingCamera = nullptr;
                    glfwTerminate();
                    openGLWindow = nullptr;
                }
            }
        }
        else
        {
            if (isGrayscale)
            {
                cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
            }
            if (isGaussian)
            {
                cv::GaussianBlur(frame, frame, cv::Size(15, 15), 0);
            }
            if (isEdge)
            {
                if (!isGrayscale)
                {
                    isGrayscale = !isGrayscale;
                    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
                }
                cv::Canny(frame, frame, 100, 200);
            }
            if (isPixelated)
            {
                cv::Mat temp;
                cv::resize(frame, temp, cv::Size(), 0.1, 0.1, cv::INTER_LINEAR);
                cv::resize(temp, frame, frame.size(), 0, 0, cv::INTER_NEAREST);
            }

            cv::Mat transformedFrame;
            cv::warpAffine(frame, transformedFrame, modelMatrixCPU, frame.size());
            cv::imshow("Live", transformedFrame);
        }
        //fps calculation
        if (resetFpsCounter)
        {
            std::cout << "Resetting FPS counter" << std::endl;
            lastTime = std::chrono::high_resolution_clock::now();
            startTime = lastTime;
            resetFpsCounter = false;
            frameCount = 0;
        }

        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count();
        double totalElapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

        if (elapsed >= 1.0)
        {
            std::cout << "Elapsed time: " << elapsed << " seconds. Frames counted: " << frameCount << " Total elapsed: " << totalElapsed << std::endl;
            double fps = frameCount / totalElapsed;
            std::cout << "FPS: " << fps << std::endl;
            lastTime = currentTime;
        }
    }

    return 0;
}

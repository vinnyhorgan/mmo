#include "Network.h"

#include <raylib.h>
#include <raymath.h>
#include <rlTPCamera.h>

#define RLIGHTS_IMPLEMENTATION
#include <rlights.h>

#include <reactphysics3d/reactphysics3d.h>

#include <iostream>
#include <vector>

rp3d::PhysicsCommon physicsCommon;
rp3d::PhysicsWorld* world;

class Ballz {
public:
    rp3d::RigidBody* body;

    Ballz() {
        rp3d::Vector3 position(0, 20, 0);
        rp3d::Quaternion orientation = rp3d::Quaternion::identity();
        rp3d::Transform transform(position, orientation);
        body = world->createRigidBody(transform);

        float radius = 0.2f;
        rp3d::SphereShape* sphereShape = physicsCommon.createSphereShape(radius);

        rp3d::Transform shapetransform = rp3d::Transform::identity();

        rp3d::Collider* collider = body->addCollider(sphereShape, shapetransform);
    }
};

std::vector<Ballz> balls;

void setTransform(Model& model, const rp3d::Transform& transform) {
    float matrix[16];
    transform.getOpenGLMatrix(matrix);

    model.transform.m0 = matrix[0];
    model.transform.m1 = matrix[1];
    model.transform.m2 = matrix[2];
    model.transform.m3 = matrix[3];
    model.transform.m4 = matrix[4];
    model.transform.m5 = matrix[5];
    model.transform.m6 = matrix[6];
    model.transform.m7 = matrix[7];
    model.transform.m8 = matrix[8];
    model.transform.m9 = matrix[9];
    model.transform.m10 = matrix[10];
    model.transform.m11 = matrix[11];
    model.transform.m12 = matrix[12];
    model.transform.m13 = matrix[13];
    model.transform.m14 = matrix[14];
    model.transform.m15 = matrix[15];
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "MMO Client by Vinny Horgan");
    SetTargetFPS(60);

    rlTPCamera camera;
    camera.Setup(45, Vector3{1, 0, 0});
    camera.ViewAngles.y = -15 * DEG2RAD;

    Vector3 ballPos = {0, 0, 0};

    Shader shader = LoadShader("assets/shaders/lighting.vs", "assets/shaders/fog.fs");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    float ambientVal[4] = {0.2, 0.2, 0.2, 1.0};
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, ambientVal, SHADER_UNIFORM_VEC4);

    float fogDensity = 0.15f;
    int fogDensityLoc = GetShaderLocation(shader, "fogDensity");
    SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    Model model = LoadModel("assets/models/robot.glb");

    // print all details of model
    std::cout << "meshCount: " << model.meshCount << std::endl;
    std::cout << "materialCount: " << model.materialCount << std::endl;
    std::cout << "boneCount: " << model.boneCount << std::endl;

    for (int i = 0; i < model.materialCount; i++) {
        model.materials[i].shader = shader;
    }

    Mesh cube = GenMeshCube(1, 1, 1);
    Model cubeModel = LoadModelFromMesh(cube);

    // print all details of model
    std::cout << "meshCount: " << cubeModel.meshCount << std::endl;
    std::cout << "materialCount: " << cubeModel.materialCount << std::endl;
    std::cout << "boneCount: " << cubeModel.boneCount << std::endl;

    cubeModel.materials[0].shader = shader;

    unsigned int animsCount = 0;
    unsigned int animIndex = 6;
    unsigned int animCurrentFrame = 0;
    ModelAnimation* anims = LoadModelAnimations("assets/models/robot.glb", &animsCount);

    std::cout << "animsCount: " << animsCount << std::endl;

    std::vector<Ray> rays;

    bool rotate = true;

    CreateLight(LIGHT_POINT, Vector3{ 0, 2, 0 }, Vector3{ 0, 0, 0 }, WHITE, shader);

    Model sph = LoadModelFromMesh(GenMeshSphere(0.2, 16, 16));
    sph.materials[0].shader = shader;

    Model floorModel = LoadModelFromMesh(GenMeshCube(200, 2, 200));
    floorModel.materials[0].shader = shader;

    // PHYSICS

    world = physicsCommon.createPhysicsWorld();

    Ballz ball1;
    balls.push_back(ball1);

    // create floor
    rp3d::Vector3 position(0, 0, 0);
    rp3d::Quaternion orientation = rp3d::Quaternion::identity();
    rp3d::Transform transform(position, orientation);
    rp3d::RigidBody* floor = world->createRigidBody(transform);
    floor->setType(rp3d::BodyType::STATIC);

    rp3d::BoxShape* boxShape = physicsCommon.createBoxShape(rp3d::Vector3(100, 1, 100));

    rp3d::Transform shapetransform = rp3d::Transform::identity();

    rp3d::Collider* collider = floor->addCollider(boxShape, shapetransform);

    setTransform(floorModel, floor->getTransform());

    // create player collider
    rp3d::Vector3 position2(0, 30, 0);
    rp3d::Quaternion orientation2 = rp3d::Quaternion::identity();
    rp3d::Transform transform2(position2, orientation2);
    rp3d::RigidBody* player = world->createRigidBody(transform2);

    rp3d::BoxShape* boxShape2 = physicsCommon.createBoxShape(rp3d::Vector3(1, 1, 1));

    rp3d::Transform shapetransform2 = rp3d::Transform::identity();

    rp3d::Collider* collider2 = player->addCollider(boxShape2, shapetransform2);

    camera.CameraPosition.y += 2;


    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        world->update(GetFrameTime());

        // iterate over balls vector


        if (IsKeyDown(KEY_LEFT))
        {
            fogDensity += 0.001f;
            if (fogDensity > 1.0f) fogDensity = 1.0f;
        }

        if (IsKeyDown(KEY_RIGHT))
        {
            fogDensity -= 0.001f;
            if (fogDensity < 0.0f) fogDensity = 0.0f;
        }

        SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &camera.CameraPosition.x, SHADER_UNIFORM_VEC3);

        camera.Update();

        rp3d::Vector3 playerpos = player->getTransform().getPosition();

        // camera.SetCameraPosition(Vector3{playerpos.x, playerpos.y + 8, playerpos.z});

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            rotate = true;
            DisableCursor();
        } else if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
            EnableCursor();
        }

        if (IsKeyPressed(KEY_LEFT_CONTROL)) {
            rotate = false;
        }

        Matrix rotMat = MatrixRotateY(camera.ViewAngles.x + PI);

        if (rotate)
            model.transform = rotMat;

        if (IsKeyPressed(KEY_UP)) animIndex = (animIndex + 1) % animsCount;
        else if (IsKeyPressed(KEY_DOWN)) animIndex = (animIndex + animsCount - 1) % animsCount;

        ModelAnimation anim = anims[animIndex];
        animCurrentFrame = (animCurrentFrame + 1) % anim.frameCount;
        UpdateModelAnimation(model, anim, animCurrentFrame);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Ray ray = camera.GetViewRay();
                rays.push_back(ray);
        }

        Ray ray = GetMouseRay(GetMousePosition(), camera.ViewCamera);

        RayCollision col = GetRayCollisionSphere(ray, ballPos, 0.2f);

        if (col.hit) {
            std::cout << "Hit!" << std::endl;
        }

        BeginDrawing();

        ClearBackground(SKYBLUE);

        camera.BeginMode3D();

        DrawModel(floorModel, Vector3{ 0, 0, 0 }, 1.0f, WHITE);

        setTransform(model, player->getTransform());
        DrawModel(model, Vector3{0, 0, 0}, 1.0f, WHITE);

        Vector3 bruh = { 0 };
        rp3d::Vector3 w = player->getTransform().getPosition();

        bruh.x = w.x;
        bruh.y = w.y;
        bruh.z = w.z;


        DrawCubeWires(bruh, 2, 2, 2, GREEN);


        if (col.hit) {
            DrawCubeWires(ballPos, 0.5, 0.5, 0.5, GREEN);
        }

        for (Ray ray : rays) {
            DrawRay(ray, RED);
        }

        for (Ballz ball : balls) {
            const rp3d::Transform& transform = ball.body->getTransform();
            setTransform(sph, transform);

            DrawModel(sph, Vector3{ 0, 0, 0 }, 1.0f, WHITE);
        }

        camera.EndMode3D();

        DrawFPS(GetScreenWidth() - 100, 10);

        DrawText(TextFormat("Animation: %i", animIndex), 10, 10, 20, BLACK);

        // draw text fog density float
        DrawText(TextFormat("Fog: %.2f", fogDensity), 10, 50, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

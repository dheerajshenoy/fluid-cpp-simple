#define SCALE 10
#define N 100

constexpr static int screenWidth = 1000;
constexpr static int screenHeight = 640;

#include "FluidCube.cpp"

int main() {

    InitWindow(screenWidth, screenHeight, "Fluid Simulation");

    FluidCube *fluid = new FluidCube(0, 0, 0.1);

    dj::Rectangle rect({ 40.0, 40.0 }, 40.0, 40.0);
    fluid->addRectangle(rect, 50.0);

    // Variables to track mouse dragging
    bool isDragging = false;
    Vector2 dragStartPos = {0};
    Vector2 dragEndPos = {0};
    Vector2 currentPos = {0};
    Vector2 prevPos = {0};

    Camera2D camera = {0};
    camera.target = (Vector2){(float) screenWidth / 2, 320}; // Center of the screen
    camera.offset = (Vector2){(float) screenWidth / 2, 320}; // Offset (where the camera "looks")
    camera.rotation = 0.0f;
    camera.zoom = 1.0f; // Scale factor (1.0 means no scaling)

    SetTargetFPS(60.0);

    while(!WindowShouldClose()) {

      // Get current mouse position
      currentPos = GetMousePosition();

      // Start dragging when the left mouse button is pressed
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        isDragging = true;
        dragStartPos = currentPos; // Capture starting position
      }

      // Stop dragging when the left mouse button is released
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        isDragging = false;
      }

      // If dragging, update the drag end position
      if (isDragging) {
        dragEndPos = currentPos;
      }

      BeginDrawing();

      if (isDragging) {
        // DrawText("Dragging...", 10, 40, 20, DARKGREEN);
        // DrawLineV(dragStartPos, dragEndPos, RED);
        // DrawCircleV(dragStartPos, 5, BLUE);
        // DrawCircleV(dragEndPos, 5, GREEN);
        // DrawCircleV(currentPos, 5, GREEN);
        fluid->addDensity( (float) currentPos.x / SCALE, (float) currentPos.y / SCALE, 100);
        Vector2 Vamt = Vector2Subtract(currentPos, prevPos);
        fluid->addVelocity( (float) currentPos.x / SCALE, (float) currentPos.y / SCALE, Vamt.x, Vamt.y);
      }

        DrawText("Click to add dye to the fluid", 20.0, 20.0, 24.0, YELLOW);
        ClearBackground(BLACK);
        BeginMode2D(camera);

        fluid->simulate();
        fluid->render();

        EndMode2D();

        EndDrawing();


        prevPos = currentPos;
    }


    CloseWindow();

    return 0;
}

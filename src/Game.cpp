#include "Game.h"
#include "raymath.h"
#include <iostream>
#include <algorithm>

const int TILE_SIZE = 64;
const int SPRITE_SCALE = 2; // Scale 32x32 sprites to 64x64

Vector2 ToIso(float x, float y) {
    return { (x - y) * (TILE_SIZE / 2.0f), (x + y) * (TILE_SIZE / 4.0f) };
}

Game::Game() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Soap Dynasty - Visual Update");
    SetTargetFPS(60);
    
    // Load 32Rogues Assets
    roguesTex = LoadTexture("assets/32rogues/32rogues/rogues.png");
    isoTex = LoadTexture("assets/isometric_tileset/isometric tileset/spritesheet.png");
    itemTex = LoadTexture("assets/32rogues/32rogues/items.png");

    playerPos = {15, 15};
    camera = {0};
    camera.offset = { SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
    camera.zoom = 1.3f;

    competitors.push_back({{5, 5}, 10.0f, "White Star"});
    criers.push_back({{20, 10}, 12.0f, "¡JABONES DE CALIDAD!"});
    
    recipes.push_back({"Jabon Barato", 10, 30, 2.0f});
    recipes.push_back({"Jabon Artesanal", 50, 150, 6.0f});
}

Game::~Game() {
    UnloadTexture(roguesTex);
    UnloadTexture(isoTex);
    UnloadTexture(itemTex);
    CloseWindow();
}

void Game::ResetDay() {
    state.dayEnded = false;
    state.goldEarnedToday = 0;
    state.soapsSoldToday = 0;
    state.lateCompetitorSpawned = false; // Reset bug flag
    clock.hours = 8;
    clock.minutes = 0;
    customers.clear();
    competitors.erase(std::remove_if(competitors.begin(), competitors.end(), [](const Competitor& c){
        return c.guild == "Chinan Case";
    }), competitors.end());
}

void Game::HandleInput() {
    if (state.dayEnded) {
        if (IsKeyPressed(KEY_ENTER)) ResetDay();
        return;
    }
    if (IsKeyPressed(KEY_C)) state.inCrafting = !state.inCrafting;
    if (!state.inCrafting) {
        Vector2 move = {0,0};
        if (IsKeyDown(KEY_W)) move.y -= 1;
        if (IsKeyDown(KEY_S)) move.y += 1;
        if (IsKeyDown(KEY_A)) move.x -= 1;
        if (IsKeyDown(KEY_D)) move.x += 1;
        if (Vector2Length(move) > 0) playerPos = Vector2Add(playerPos, Vector2Scale(Vector2Normalize(move), 6.5f * GetFrameTime()));
    } else {
        if (IsKeyPressed(KEY_ONE)) state.selectedRecipe = 0;
        if (IsKeyPressed(KEY_TWO)) state.selectedRecipe = 1;
        if (IsKeyPressed(KEY_F) && !state.isCrafting) {
            if (state.gold >= recipes[state.selectedRecipe].cost) {
                state.gold -= recipes[state.selectedRecipe].cost;
                state.isCrafting = true;
                state.craftingProgress = 0.0f;
            }
        }
    }
}

void Game::Update() {
    if (state.dayEnded) return;
    float dt = GetFrameTime();
    clock.Update(dt);

    if (clock.hours >= 20) { state.dayEnded = true; return; }

    // Fix spawning bug with flag
    if (clock.hours >= 14 && !state.lateCompetitorSpawned) {
        state.lateCompetitorSpawned = true;
        competitors.push_back({{30, 10}, 14.0f, "Chinan Case"});
    }

    if (state.isCrafting) {
        state.craftingProgress += (1.0f / recipes[state.selectedRecipe].time) * dt;
        if (state.craftingProgress >= 1.0f) { state.isCrafting = false; state.inventory[state.selectedRecipe]++; }
    }

    state.spawnTimer += dt;
    if (state.spawnTimer >= 3.5f) {
        state.spawnTimer = 0;
        if (customers.size() < 18) {
            int type = 0;
            if (state.reputation > 80) type = GetRandomValue(0, 2);
            else if (state.reputation > 40) type = GetRandomValue(0, 1);
            
            // Assign sprite based on type from roguesTex (32x32 tiles)
            Rectangle rect;
            if (type == 2) rect = { 32 * 1, 0, 32, 32 }; // Knight sprite
            else if (type == 1) rect = { 32 * 2, 0, 32, 32 }; // Merchant sprite
            else rect = { 32 * 0, 0, 32, 32 }; // Poor sprite
            
            customers.push_back({{ (float)GetRandomValue(5, 35), (float)GetRandomValue(5, 35) }, 100.0f, false, false, type, 2.2f, 0, 0.0f, rect});
        }
    }

    for (int i = 0; i < (int)customers.size(); i++) {
        auto& c = customers[i];
        Vector2 target = playerPos;
        bool stolen = false;
        bool attracted = false;

        for (auto& comp : competitors) {
            if (Vector2Distance(c.pos, comp.pos) < comp.influenceRadius) {
                target = comp.pos;
                stolen = true;
                state.reputation -= 2.0f * dt;
                break;
            }
        }
        if (!stolen) {
            for (auto& cr : criers) {
                if (Vector2Distance(c.pos, cr.pos) < cr.radius) { attracted = true; break; }
            }
        }

        Vector2 dir = Vector2Normalize(Vector2Subtract(target, c.pos));
        float spd = (stolen || attracted) ? c.speed : c.speed * 0.4f;
        c.pos = Vector2Add(c.pos, Vector2Scale(dir, spd * dt));

        if (Vector2Distance(c.pos, playerPos) < 2.5f) {
            c.isWaiting = true;
            c.patience -= 12.0f * dt;
        } else c.isWaiting = false;
    }

    if (IsKeyDown(KEY_E)) {
        bool found = false;
        for (int i = 0; i < (int)customers.size(); i++) {
            auto& c = customers[i];
            int soapType = (c.type == 2) ? 1 : 0;
            if (Vector2Distance(c.pos, playerPos) < 2.0f && !c.satisfied && c.patience > 0 && state.inventory[soapType] > 0) {
                state.currentTargetIndex = i;
                state.sellingProgress += 1.25f * dt;
                if (state.sellingProgress >= 1.0f) {
                    state.sellingProgress = 0;
                    c.satisfied = true;
                    state.inventory[soapType]--;
                    state.gold += recipes[soapType].price;
                    state.goldEarnedToday += recipes[soapType].price;
                    state.soapsSoldToday++;
                    state.reputation += (c.type == 2) ? 20.0f : 6.0f;
                }
                found = true;
                break;
            }
        }
        if (!found) state.currentTargetIndex = -1;
    } else { state.sellingProgress = 0; state.currentTargetIndex = -1; }

    customers.erase(std::remove_if(customers.begin(), customers.end(), [](const Customer& c) { return c.satisfied || c.patience <= 0; }), customers.end());
    camera.target = ToIso(playerPos.x, playerPos.y);
}

void Game::Draw() {
    BeginDrawing();
    Color skyColor = (clock.hours >= 19 || clock.hours < 6) ? Color{10, 10, 25, 255} : Color{70, 85, 110, 255};
    ClearBackground(skyColor);

    BeginMode2D(camera);
        // Draw World Tiles with identity
        const int TS = 44; // tile size en spritesheet
        for(int x = 0; x < 40; x++) {
            for(int y = 0; y < 40; y++) {
                Vector2 iso = ToIso(x, y);

                Rectangle src;
                // Plaza de mercado = pavimento (fila 8)
                if (x > 10 && x < 20 && y > 10 && y < 20)
                    src = { 0, (float)TS * 8, (float)TS, (float)TS };
                // Camino diagonal = tierra (fila 0)
                else if (abs(x - y) < 2)
                    src = { 0, (float)TS * 0, (float)TS, (float)TS };
                // Resto = pasto (fila 2)
                else
                    src = { 0, (float)TS * 2, (float)TS, (float)TS };

                DrawTexturePro(isoTex, src,
                    { iso.x - TS/2.0f, iso.y - TS/2.0f, (float)TS, (float)TS },
                    { 0, 0 }, 0, WHITE);
            }
        }

        for(auto& cr : criers) {
            Vector2 iso = ToIso(cr.pos.x, cr.pos.y);
            DrawCircleLinesV(iso, cr.radius * 25, Fade(YELLOW, 0.4f));
            DrawText("¡PREGONERO!", iso.x - 30, iso.y - 60, 10, YELLOW);
        }

        // Draw Competitors as Buildings
        for(auto& comp : competitors) {
            Vector2 iso = ToIso(comp.pos.x, comp.pos.y);
            DrawRectangle(iso.x - 40, iso.y - 120, 80, 120, DARKGRAY); // Building body
            DrawRectangle(iso.x - 45, iso.y - 130, 90, 20, MAROON); // Roof
            DrawCircleLinesV(iso, comp.influenceRadius * 25, Fade(RED, 0.4f));
            DrawText(comp.guild.c_str(), iso.x - 40, iso.y - 150, 15, RED);
        }

        // Draw Customers (Rogues)
        for(auto& c : customers) {
            Vector2 iso = ToIso(c.pos.x, c.pos.y);
            DrawTexturePro(roguesTex, c.spriteRect, {iso.x, iso.y, 64, 64}, {32, 64}, 0, WHITE);
            if (c.isWaiting) {
                DrawRectangle(iso.x - 20, iso.y - 75, 40, 4, BLACK);
                DrawRectangle(iso.x - 20, iso.y - 75, (int)(40 * c.patience/100.0f), 4, LIME);
            }
        }

        // Draw Player (Special Rogue)
        Vector2 pIso = ToIso(playerPos.x, playerPos.y);
        DrawTexturePro(roguesTex, {32*3, 32*1, 32, 32}, {pIso.x, pIso.y, 64, 64}, {32, 64}, 0, WHITE);
        
        if (state.currentTargetIndex != -1) {
            DrawRectangle(pIso.x - 30, pIso.y - 85, 60, 6, BLACK);
            DrawRectangle(pIso.x - 30, pIso.y - 85, (int)(60 * state.sellingProgress), 6, GOLD);
        }
    EndMode2D();
    
    DrawHUD();

    if (state.dayEnded) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.85f));
        DrawText("--- BALANCE DEL DIA ---", SCREEN_WIDTH/2 - 180, 200, 32, GOLD);
        DrawText(TextFormat("Oro Ganado: %d ¥", state.goldEarnedToday), SCREEN_WIDTH/2 - 100, 280, 24, RAYWHITE);
        DrawText(TextFormat("Jabones Vendidos: %d", state.soapsSoldToday), SCREEN_WIDTH/2 - 100, 320, 24, RAYWHITE);
        DrawText("PRESIONA ENTER PARA OTRO DIA", SCREEN_WIDTH/2 - 200, 500, 20, YELLOW);
    }
    EndDrawing();
}

void Game::DrawHUD() {
    // Top Bar Background
    DrawRectangle(0, 0, SCREEN_WIDTH, 70, Fade(BLACK, 0.8f));
    DrawRectangleLines(0, 0, SCREEN_WIDTH, 70, DARKGRAY);

    // Stats with icons/colors
    DrawText("MONEDA:", 20, 25, 18, GRAY);
    DrawText(TextFormat("%d ¥", state.gold), 110, 22, 24, GOLD);

    DrawText("REP:", 250, 25, 18, GRAY);
    DrawText(TextFormat("%.0f", state.reputation), 300, 22, 24, SKYBLUE);

    DrawText("INVENTARIO:", 450, 25, 18, GRAY);
    DrawText(TextFormat("B:%d | A:%d", state.inventory[0], state.inventory[1]), 560, 22, 24, RAYWHITE);

    // Clock
    DrawRectangle(SCREEN_WIDTH - 160, 10, 140, 50, DARKGRAY);
    DrawText(clock.GetTime().c_str(), SCREEN_WIDTH - 145, 15, 38, YELLOW);

    if (state.inCrafting) {
        DrawRectangle(SCREEN_WIDTH/2 - 300, 150, 600, 400, Fade(BLACK, 0.95f));
        DrawRectangleLines(SCREEN_WIDTH/2 - 300, 150, 600, 400, GOLD);
        DrawText("ALQUIMIA IMPERIAL", SCREEN_WIDTH/2 - 120, 180, 24, GOLD);
        for(int i=0; i<recipes.size(); i++) {
            Color col = (state.selectedRecipe == i) ? LIME : GRAY;
            DrawText(TextFormat("%d. %s (%d ¥)", i+1, recipes[i].name.c_str(), recipes[i].cost), SCREEN_WIDTH/2 - 250, 250 + i*60, 20, col);
        }
        if (state.isCrafting) {
            DrawRectangle(SCREEN_WIDTH/2 - 250, 400, 500, 20, DARKGRAY);
            DrawRectangle(SCREEN_WIDTH/2 - 250, 400, (int)(500 * state.craftingProgress), 20, GOLD);
            DrawText("TRABAJANDO...", SCREEN_WIDTH/2 - 60, 375, 18, GOLD);
        }
    }
}

void Game::Run() {
    while (!WindowShouldClose()) {
        HandleInput();
        Update();
        Draw();
    }
}

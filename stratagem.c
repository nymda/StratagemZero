#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <gui/elements.h>
#include <gui/canvas.h>
#include <stdlib.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include "tinyfont.h"
/* generated by fbt from .png files in images folder */
#include <stratagem_icons.h>

#define TIME_PENALTY_MULTIPLIER 0.10
#define TIME_REWARD_MULTIPLIER 0.10
#define TIME_DECREASE_MULTIPLER 0.25

typedef struct{
    char name[24];
    char arrows[12];
} stratagem;

typedef struct{
    char name[32];
    int groupID;
    stratagem stratagems[16];
    int stratagemCount;
} group;

typedef enum{
    begin,
    active,
    end,
} state;

group groups[6] = {};

NotificationApp* notif = 0;

int32_t totaltime = 2000;
int32_t remainingtime = 2000;
int8_t groupIndex = 0;
int8_t strataIndex = 0;
int8_t arrowIndex = 0;
int8_t score = 0;
int8_t level = 0;
int8_t levelLen = 0;
int8_t levelCompleted = 0;
state gameState = begin;

char levelText[16];
char scoreText[16];

void nextStrata(){
    int nextIndex = furi_hal_random_get() % groups[groupIndex].stratagemCount;
    while(strataIndex == nextIndex){ nextIndex = furi_hal_random_get() % groups[groupIndex].stratagemCount; }
    strataIndex = nextIndex;
}

void nextLevel(){
    level++;
    levelLen = (furi_hal_random_get() % 4) + 4;
    levelCompleted = 0;
    totaltime -= ((float)totaltime * TIME_DECREASE_MULTIPLER);
    remainingtime = totaltime;

    int nextIndex = furi_hal_random_get() % 6;
    while(groupIndex == nextIndex){ nextIndex = furi_hal_random_get() % 6; }
    groupIndex = nextIndex;

    nextStrata();
}

void newGame(){
    totaltime = 2000;
    remainingtime = 2000;
    groupIndex = 0;
    strataIndex = 0;
    arrowIndex = 0;
    level = 0;
    levelLen = 0;
    levelCompleted = 0;
    nextLevel();
}

void initStratagems(){
    groups[0].groupID = 0;
    groups[0].stratagemCount = 3;
    memcpy(groups[0].name, "Mission", 8);

    memcpy(groups[0].stratagems[0].name, "Reinforce", 10);
    memcpy(groups[0].stratagems[0].arrows, "UDRLU", 6);

    memcpy(groups[0].stratagems[1].name, "SOS Beacon", 11);
    memcpy(groups[0].stratagems[1].arrows, "UDLR", 5);

    memcpy(groups[0].stratagems[2].name, "Resupply", 9);
    memcpy(groups[0].stratagems[2].arrows, "DDUR", 5);

    groups[1].groupID = 1;
    groups[1].stratagemCount = 8;
    memcpy(groups[1].name, "Offensive: Eagle", 17);

    memcpy(groups[1].stratagems[0].name, "Eagle Rearm", 12);
    memcpy(groups[1].stratagems[0].arrows, "UULUR", 6);

    memcpy(groups[1].stratagems[1].name, "Eagle Strafing Run", 19);
    memcpy(groups[1].stratagems[1].arrows, "URR", 4);

    memcpy(groups[1].stratagems[2].name, "Eagle Airstrike", 16);
    memcpy(groups[1].stratagems[2].arrows, "URDR", 5);

    memcpy(groups[1].stratagems[3].name, "Eagle Cluster Bomb", 19);
    memcpy(groups[1].stratagems[3].arrows, "URDDR", 6);

    memcpy(groups[1].stratagems[4].name, "Eagle Napalm Strike", 20);
    memcpy(groups[1].stratagems[4].arrows, "URDU", 5);

    memcpy(groups[1].stratagems[5].name, "Eagle Smoke Strike", 19);
    memcpy(groups[1].stratagems[5].arrows, "URUD", 5);

    memcpy(groups[1].stratagems[6].name, "Eagle 110MM Rockets", 20);
    memcpy(groups[1].stratagems[6].arrows, "URUL", 5);

    memcpy(groups[1].stratagems[7].name, "Eagle 500kg Bomb", 17);
    memcpy(groups[1].stratagems[7].arrows, "URDDD", 6);

    groups[2].groupID = 2;
    groups[2].stratagemCount = 15;
    memcpy(groups[2].name, "Supply: Weapons", 16);

    memcpy(groups[2].stratagems[0].name, "RL-77 Rocket Launcher", 22);
    memcpy(groups[2].stratagems[0].arrows, "DUULR", 6);

    memcpy(groups[2].stratagems[1].name, "AC-8 Autocannon", 16);
    memcpy(groups[2].stratagems[1].arrows, "DLDUUR", 7);

    memcpy(groups[2].stratagems[2].name, "EAT-17 Anti-Tank", 17);
    memcpy(groups[2].stratagems[2].arrows, "DDLUR", 6);

    memcpy(groups[2].stratagems[3].name, "FLAM-40 Flamethrower", 21);
    memcpy(groups[2].stratagems[3].arrows, "DLDRL", 6);

    memcpy(groups[2].stratagems[4].name, "LAS-98 Laser Cannon", 20);
    memcpy(groups[2].stratagems[4].arrows, "DLDUL", 6);

    memcpy(groups[2].stratagems[5].name, "M-105 Stalwart", 15);
    memcpy(groups[2].stratagems[5].arrows, "DLDUUL", 7);

    memcpy(groups[2].stratagems[6].name, "MG-43 Machine Gun", 18);
    memcpy(groups[2].stratagems[6].arrows, "DLDUR", 6);

    memcpy(groups[2].stratagems[7].name, "ARC-3 Arc Thrower", 18);
    memcpy(groups[2].stratagems[7].arrows, "DRDULL", 7);

    memcpy(groups[2].stratagems[8].name, "GL-21 Grenade Launcher", 18);
    memcpy(groups[2].stratagems[8].arrows, "DLULD", 6);

    memcpy(groups[2].stratagems[9].name, "APW-1 Anti-Mat Rifle", 21);
    memcpy(groups[2].stratagems[9].arrows, "DLRUD", 6);

    memcpy(groups[2].stratagems[10].name, "RS-422 Railgun", 15);
    memcpy(groups[2].stratagems[10].arrows, "DRDULR", 7);

    memcpy(groups[2].stratagems[11].name, "GR-8 Recoilless Rifle", 22);
    memcpy(groups[2].stratagems[11].arrows, "DLRRL", 6);

    memcpy(groups[2].stratagems[12].name, "FAF-14 Spear", 13);
    memcpy(groups[2].stratagems[12].arrows, "DDUDD", 6);

    memcpy(groups[2].stratagems[13].name, "LAS-99 Quasar Cannon", 21);
    memcpy(groups[2].stratagems[13].arrows, "DDULR", 6);

    memcpy(groups[2].stratagems[14].name, "MG-206 Machine Gun", 19);
    memcpy(groups[2].stratagems[14].arrows, "DLUDD", 6);

    groups[3].groupID = 3;
    groups[3].stratagemCount = 11;
    memcpy(groups[3].name, "Offensive: Orbital", 19);

    memcpy(groups[3].stratagems[0].name, "O. Precision Strike", 20);
    memcpy(groups[3].stratagems[0].arrows, "RRU", 4);

    memcpy(groups[3].stratagems[1].name, "O. Airbust Strike", 18);
    memcpy(groups[3].stratagems[1].arrows, "RRR", 4);

    memcpy(groups[3].stratagems[2].name, "O. 120MM HE Barrage", 20);
    memcpy(groups[3].stratagems[2].arrows, "RDDLDRDD", 9);

    memcpy(groups[3].stratagems[3].name, "O. 380MM HE Barrage", 20);
    memcpy(groups[3].stratagems[3].arrows, "RDUULDD", 8);

    memcpy(groups[3].stratagems[4].name, "O. Walking Barrage", 19);
    memcpy(groups[3].stratagems[4].arrows, "RDRDRD", 7);

    memcpy(groups[3].stratagems[5].name, "Orbital Laser", 14);
    memcpy(groups[3].stratagems[5].arrows, "RDURD", 6);

    memcpy(groups[3].stratagems[6].name, "O. Railcannon Strike", 21);
    memcpy(groups[3].stratagems[6].arrows, "RUDDR", 6);

    memcpy(groups[3].stratagems[7].name, "O. Gatling Barrage", 19);
    memcpy(groups[3].stratagems[7].arrows, "RDLUU", 6);

    memcpy(groups[3].stratagems[8].name, "O. Gas Strike", 14);
    memcpy(groups[3].stratagems[8].arrows, "RRDR", 5);

    memcpy(groups[3].stratagems[9].name, "O. EMS Strike", 14);
    memcpy(groups[3].stratagems[9].arrows, "RRLD", 5);

    memcpy(groups[3].stratagems[10].name, "O. Smoke Strike", 16);
    memcpy(groups[3].stratagems[10].arrows, "RRDU", 5);

    groups[4].groupID = 4;
    groups[4].stratagemCount = 7;
    memcpy(groups[4].name, "Supply: Extras", 15);

    memcpy(groups[4].stratagems[0].name, "AX/LAS-5 Rover", 15);
    memcpy(groups[4].stratagems[0].arrows, "DULURR", 7);

    memcpy(groups[4].stratagems[1].name, "AD-334 Guard Dog", 17);
    memcpy(groups[4].stratagems[1].arrows, "DULURD", 7);

    memcpy(groups[4].stratagems[2].name, "LIFT-850 Jump Pack", 19);
    memcpy(groups[4].stratagems[2].arrows, "DUUDU", 6);

    memcpy(groups[4].stratagems[3].name, "B-1 Supply Pack", 16);
    memcpy(groups[4].stratagems[3].arrows, "DUDDR", 6);

    memcpy(groups[4].stratagems[4].name, "SH-20 Generator", 16);
    memcpy(groups[4].stratagems[4].arrows, "DULRLR", 7);

    memcpy(groups[4].stratagems[5].name, "SH-20 Ballistic", 16);
    memcpy(groups[4].stratagems[5].arrows, "DLDDUL", 7);

    memcpy(groups[4].stratagems[6].name, "EXO-45 Exosuit", 15);
    memcpy(groups[4].stratagems[6].arrows, "LDRULDD", 8);

    groups[5].groupID = 5;
    groups[5].stratagemCount = 11;
    memcpy(groups[5].name, "Defensive", 10);
    
    memcpy(groups[5].stratagems[0].name, "Machine Gun Sentry", 19);
    memcpy(groups[5].stratagems[0].arrows, "DURRU", 6);

    memcpy(groups[5].stratagems[1].name, "Mortar Sentry", 14);
    memcpy(groups[5].stratagems[1].arrows, "DURRD", 6);

    memcpy(groups[5].stratagems[2].name, "EMS Mortar Sentry", 18);
    memcpy(groups[5].stratagems[2].arrows, "DURDR", 6);

    memcpy(groups[5].stratagems[3].name, "Gatling Sentry", 15);
    memcpy(groups[5].stratagems[3].arrows, "DURL", 5);

    memcpy(groups[5].stratagems[4].name, "Autocannon Sentry", 18);
    memcpy(groups[5].stratagems[4].arrows, "DURULU", 7);

    memcpy(groups[5].stratagems[5].name, "Rocket Sentry", 14);
    memcpy(groups[5].stratagems[5].arrows, "DURRL", 6);
    
    memcpy(groups[5].stratagems[6].name, "Tesla Tower", 12);
    memcpy(groups[5].stratagems[6].arrows, "DURULR", 7);

    memcpy(groups[5].stratagems[7].name, "MD-6 Minefield", 15);
    memcpy(groups[5].stratagems[7].arrows, "DLUR", 5);

    memcpy(groups[5].stratagems[8].name, "MD-I4 Minefield", 16);
    memcpy(groups[5].stratagems[8].arrows, "DLLD", 5);

    memcpy(groups[5].stratagems[9].name, "FX-12 Generator", 16);
    memcpy(groups[5].stratagems[9].arrows, "DDLRLR", 7);

    memcpy(groups[5].stratagems[10].name, "E/MG-101 HMG", 13);
    memcpy(groups[5].stratagems[10].arrows, "DRLRRL", 7);
}

void draw_stratagem(Canvas* canvas, uint8_t y){
    int arrowSize = 14; //its square
    int x = 64 - (((int)strlen(groups[groupIndex].stratagems[strataIndex].arrows) * arrowSize) / 2);
    y -= arrowSize / 2;

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, y - (arrowSize * 0.5) - 12, AlignCenter, AlignCenter, groups[groupIndex].name);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, y - (arrowSize * 0.5), AlignCenter, AlignCenter, groups[groupIndex].stratagems[strataIndex].name);

    for(int i = 0; i < (int)strlen(groups[groupIndex].stratagems[strataIndex].arrows); i++){
        switch (groups[groupIndex].stratagems[strataIndex].arrows[i])
        {
            case 'U':
                if(arrowIndex > i){ canvas_draw_icon(canvas, x, y, &I_UpArrow_Filled_14x14); }
                else{ canvas_draw_icon(canvas, x, y, &I_UpArrow_Empty_14x14); }
                break;

            case 'D':
                if(arrowIndex > i){ canvas_draw_icon(canvas, x, y, &I_DownArrow_Filled_14x14); }
                else{ canvas_draw_icon(canvas, x, y, &I_DownArrow_Empty_14x14); }
                break;

            case 'L':
                if(arrowIndex > i){ canvas_draw_icon(canvas, x, y, &I_LeftArrow_Filled_14x14); }
                else{ canvas_draw_icon(canvas, x, y, &I_LeftArrow_Empty_14x14); }
                break;

            case 'R':
                if(arrowIndex > i){ canvas_draw_icon(canvas, x, y, &I_RightArrow_Filled_14x14); }
                else{ canvas_draw_icon(canvas, x, y, &I_RightArrow_Empty_14x14); }
                break;
            
            case 0x00:
                return;
        }
        x += arrowSize;
    }
}

void draw_timer_bar_center(Canvas* canvas, uint8_t x, uint8_t y, uint8_t width, float progress) {
    furi_assert(canvas);
    furi_assert((progress >= 0.0) && (progress <= 1.0));
    uint8_t height = 9;

    uint8_t progress_length = roundf(progress * (width - 2));

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x + 1, y + 1, width - 2, height - 2);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, x, y, width, height);

    uint8_t midpoint = x + (width / 2);

    canvas_draw_box(canvas, midpoint - (progress_length / 2), y + 1, progress_length, height - 2);
}

void draw_timer_bar(Canvas* canvas, uint8_t x, uint8_t y, uint8_t width, float progress) {
    furi_assert(canvas);
    furi_assert((progress >= 0.0) && (progress <= 1.0));
    uint8_t height = 9;

    uint8_t progress_length = roundf(progress * (width - 2));

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x + 1, y + 1, width - 2, height - 2);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, x, y, width, height);

    canvas_draw_box(canvas, x + 1, y + 1, progress_length, height - 2);
}

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);

    if(gameState == active || gameState == begin){
        draw_stratagem(canvas, 40);
        float remPerc = (float)((float)remainingtime / (float)totaltime);
        draw_timer_bar(canvas, 2, 53, 124, remPerc);
    }
    if(gameState == end){
        snprintf(levelText, 16, "Level: %i", level);
        snprintf(scoreText, 16, "Score: %i", score);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignCenter, "Game Over");

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 34, AlignCenter, AlignCenter, levelText);
        canvas_draw_str_aligned(canvas, 64, 44, AlignCenter, AlignCenter, scoreText);
    }
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, 100);
}

static void timer_callback(void* ctx) {
    furi_assert(ctx);
    UNUSED(ctx);

    if(gameState == begin || gameState == end){ return; }

    if(remainingtime <= 0){ 
        remainingtime = 0;
        gameState = end;
        notification_message(notif, &sequence_error);
        return; 
    }
    remainingtime--;
}

static void handleKeyPress(InputKey key){
    bool fail = false;

    if(gameState == end){ 
        gameState = begin; 
        newGame();
        return;
    }
    else if(gameState == begin){
        gameState = active;
    }

    if(key == InputKeyUp){
        if(groups[groupIndex].stratagems[strataIndex].arrows[arrowIndex] == 'U'){
            arrowIndex++;
        }
        else{ fail = true; }
    }
    if(key == InputKeyDown){
        if(groups[groupIndex].stratagems[strataIndex].arrows[arrowIndex] == 'D'){
            arrowIndex++;
        }
        else{ fail = true; }
    }
    if(key == InputKeyLeft){
        if(groups[groupIndex].stratagems[strataIndex].arrows[arrowIndex] == 'L'){
            arrowIndex++;
        }
        else{ fail = true; }
    }
    if(key == InputKeyRight){
        if(groups[groupIndex].stratagems[strataIndex].arrows[arrowIndex] == 'R'){
            arrowIndex++;
        }
        else{ fail = true; }
    }

    if(fail){
        remainingtime -= ((float)totaltime * TIME_PENALTY_MULTIPLIER);;
        arrowIndex = 0;
        notification_message(notif, &sequence_single_vibro);
    }

    if(arrowIndex == ((int)strlen(groups[groupIndex].stratagems[strataIndex].arrows))){
        arrowIndex = 0;
        remainingtime += ((float)totaltime * TIME_REWARD_MULTIPLIER);
        if(remainingtime > totaltime){ remainingtime = totaltime; }
        levelCompleted++;

        if(levelCompleted == levelLen){
            nextLevel();
            notification_message(notif, &sequence_double_vibro);
        }
        else{
            nextStrata();
            score++;
        }
    }
}

int32_t stratagem_app(void* p) {
    UNUSED(p);

    furi_hal_random_init();
    initStratagems();

    InputEvent event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, NULL);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    FuriTimer* timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, NULL);
    furi_timer_start(timer, (uint32_t)furi_kernel_get_tick_frequency() / 100);

    notif = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notif, &sequence_display_backlight_on);

    bool running = true;

    view_port_update(view_port);

    newGame();

    while(running) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        view_port_update(view_port);
        if(!(event_status == FuriStatusOk)){ continue; }
        if(event.type == InputTypePress) {
            if(event.key == InputKeyUp || event.key == InputKeyDown || event.key == InputKeyLeft || event.key == InputKeyRight || event.key == InputKeyOk){
                handleKeyPress(event.key);
            }
            if(event.key == InputKeyBack) { running = false; }
        }
    }

    furi_timer_stop(timer);
    furi_timer_free(timer);
    furi_message_queue_free(event_queue);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    return 0;
}

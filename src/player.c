#include <raylib.h>
#include <rlgl.h>

#include "player.h"

void player_init(Player* player) {
    player->position = (Vec3) {0.0f, 1.7f, 0.0f};
    player->speed = PLAYER_SPEED;
    player->velocity = (Vec3) {0.0f, 0.0f, 0.0f};
    player->input_direction = (Vec3) {0.0f, 0.0f, 0.0f};
    player->rotation = 0.0f;
    player->color = PLAYER_COLOR;
}

void player_update(Player* player, f32* camera_yaw, f32 delta_time) {
    player->rotation = *camera_yaw;
    Vec3 target_velocity = {
        player->input_direction.x * player->speed,
        player->input_direction.y * player->speed,
        player->input_direction.z * player->speed
    };
    f32 acceleration_rate = PLAYER_ACC;
    f32 lerp_factor = acceleration_rate * delta_time;
    if (lerp_factor > 1.0f) {
        lerp_factor = 1.0f;
    }
    player->velocity = Vector3Lerp(player->velocity, target_velocity, lerp_factor);

    player->position.x += player->velocity.x * delta_time;
    player->position.y += player->velocity.y * delta_time;
    player->position.z += player->velocity.z * delta_time;
}

void player_render(Player* player) {
    rlPushMatrix();
    rlTranslatef(player->position.x, player->position.y, player->position.z);
    rlRotatef(player->rotation * RAD2DEG, 0.0f, 1.0f, 0.0f);
    rlPopMatrix();
}
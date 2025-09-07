This shows the joystick mapping I checked. If you need to know the state of a certain button, you can subscribe to joy node and get the button state  

auto axis_left_x = -msg->axes[0];
auto axis_left_y = msg->axes[1];
auto axis_right_x = msg->axes[3];
auto axis_right_y = msg->axes[4];

auto btn_cross = msg->buttons[0];
auto btn_circle = msg->buttons[1];
auto btn_triangle = msg->buttons[2];
auto btn_square = msg->buttons[3];

auto btn_lmenu = msg->buttons[8];
auto btn_mmenu = msg->buttons[10];
auto btn_rmenu = msg->buttons[9];

auto btn_lstick = msg->buttons[11];
auto btn_rstick = msg->buttons[12];

auto btn_l1 = msg->buttons[4];
auto btn_r1 = msg->buttons[5];
auto btn_l2 = -(msg->axes[2] - 1) / 2;
auto btn_r2 = -(msg->axes[5] - 1) / 2;

int btn_left = 0;
int btn_right = 0;
int btn_up = 0;
int btn_down = 0;

auto dpad_ud = msg->axes[7];
auto dpad_lr = msg->axes[6];

btn_up = dpad_ud == 1;
btn_down = dpad_ud == -1;
btn_left = dpad_lr == 1;
btn_right = dpad_lr == -1;

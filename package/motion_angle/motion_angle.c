/**
 * @file motion_angle.c
 * @author Yu-An Chen
 * @brief calculate angle from motion sensor
 * @version 1.0
 * @date 2023-06-01
 *
 * make motion_angle-dirclean && make
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define VERSION "1.0"
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

#define MOTION_PATH_SIZE 60
#define MOTION_DATA_SIZE 10
#define MOTION_AXIS_1G 1000
#define MOTION_AXIS_MAX_NOISE 200
#define MOTION_AXIS_MAX (MOTION_AXIS_1G + MOTION_AXIS_MAX_NOISE)
#define ANGLE_ERROR 500

/* default iio device number of motion sensor is 0 */
static char iiodevice[] = "/sys/bus/iio/devices/iio:device0";
static char target_path[MOTION_PATH_SIZE];

#define TRUE 1
#define FALSE 0

#define NORMAL      LOG_LEVEL_NORMAL
#define DEBUG       LOG_LEVEL_DEBUG

#define __printf(level, fmt, ...) \
           do { if (level <= mode) printf(fmt, __VA_ARGS__); } while (0)

#define __prints(level,...) \
           do { if (level <= mode) printf(__VA_ARGS__); } while (0)

#define DBGF(...) __printf(DEBUG,__VA_ARGS__)
#define DBG(...)  __prints(DEBUG,__VA_ARGS__)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


/* log level */
enum log_level_list {
    LOG_LEVEL_NORMAL = 0,    /* normal info */
    LOG_LEVEL_DEBUG,
};

/* default log mode */
static int mode = LOG_LEVEL_NORMAL;

static int print_all = FALSE;

/* help string */
const char str_help[] =
    "\nmotion_angle\n\n"
    " About:\n"
    "    @author Yu-An Chen\n"
    "    @brief calculate angle from motion sensor.\n"
    "    @version 1.0\n"
    "    @date 2023-07-03\n";

const char str_how_to_use[] =
    " How to use:\n"
    "    motion_angle           , calculate and print motion angle\n"
    "    motion_angle -n 1      , select iio:device1, default is iio:device0\n"
    "    motion_angle -g        , print raw motion sensor value\n"
    "    motion_angle -f <freq> , set sensor sampling frequency\n"
    "\n"
    " Argument:\n"
    "    -a , print motion angle and motion sensor value\n"
    "    -d , enable debug print\n"
    "    -f , set motion sensor sampling frequency\n"
    "    -g , print raw motion sensor value\n"
    "    -h , print help string\n"
    "    -n , select iio device number, default is iio:device0\n"
    "    -v , Print tool verison\n"
    "\n";

void help(void)
{
    printf("%s\n%s", str_help, str_how_to_use);
    return;
}

int set_sampling_freq(int freq)
{
    FILE* fp;
    char buffer[10];

    sprintf(buffer, "%d", freq);
    sprintf(target_path, "%s/%s", iiodevice, "sampling_frequency");

    DBGF("Set sampling freq:%s\n", buffer);

    fp = fopen(target_path, "w");

    if (fp == NULL) {
        printf("%s: %s not found or permission denied.\n", __func__, target_path);
        return -1;
    }

    fwrite(buffer, sizeof(char), ARRAY_SIZE(buffer), fp);

    fclose(fp);

    return 0;
}

int iiodevice_read_int(const char *target, int *out, int size)
{
    FILE* fp;
    char buffer[size];
    int value;

    sprintf(target_path, "%s/%s", iiodevice, target);

    /* Read file system */
    fp = fopen(target_path, "rb");

    if (fp == NULL) {
        printf("%s: %s not found or permission denied.\n", __func__, target_path);
        return -1;
    }

    memset(buffer, 'a', ARRAY_SIZE(buffer));

    fread(buffer, strlen(buffer)+1, 1, fp);

    value = atoi(buffer);

    *out = value;

    fclose(fp);
    return 0;
}

int iiodevice_read_float(const char *target, float *out, int size)
{
    FILE* fp;
    char buffer[size];
    float value;

    sprintf(target_path, "%s/%s", iiodevice, target);

    /* Read file system */
    fp = fopen(target_path, "r");

    if (fp == NULL) {
        printf("%s: %s not found or permission denied.\n", __func__, target_path);
        return -1;
    }

    memset(buffer, 'a', ARRAY_SIZE(buffer));

    fread(buffer, strlen(buffer)+1, 1, fp);

    value = (float) atof(buffer);

    *out = value;

    fclose(fp);
    return 0;
}

int get_motion_scale(float *scale_x, float *scale_y, float *scale_z)
{
    float val_x, val_y, val_z;
    int rv;

    rv = iiodevice_read_float("in_accel_x_scale", &val_x, MOTION_DATA_SIZE);
    if (rv != 0) {
        printf("%s: fail to read motion sensor value!\n", __func__);
        return -1;
    }

    *scale_x = val_x;

    rv = iiodevice_read_float("in_accel_y_scale", &val_y, MOTION_DATA_SIZE);
    if (rv != 0) {
        printf("%s: fail to read motion sensor value!\n", __func__);
        return -1;
    }

    *scale_y = val_y;

    rv = iiodevice_read_float("in_accel_z_scale", &val_z, MOTION_DATA_SIZE);
    if (rv != 0) {
        printf("%s: fail to read motion sensor value!\n", __func__);
        return -1;
    }

    *scale_z = val_z;

    return 0;
}


int get_motion_data(int *x, int *y, int *z)
{
    int val_x, val_y, val_z;
    int rv;

    rv = iiodevice_read_int("in_accel_x_raw", &val_x, MOTION_DATA_SIZE);
    if (rv != 0) {
        printf("%s: fail to read motion sensor value!\n", __func__);
        return -1;
    }

    *x = val_x;

    rv = iiodevice_read_int("in_accel_y_raw", &val_y, MOTION_DATA_SIZE);
    if (rv != 0) {
        printf("%s: fail to read motion sensor value!\n", __func__);
        return -1;
    }

    *y = val_y;

    rv = iiodevice_read_int("in_accel_z_raw", &val_z, MOTION_DATA_SIZE);
    if (rv != 0) {
        printf("%s: fail to read motion sensor value!\n", __func__);
        return -1;
    }

    *z = val_z;

    DBGF("X=%d, Y=%d, Z=%d\n", *x, *y, *z);

    return 0;
}

int calculate_motion_angle(int accel_x, int accel_y, int accel_z)
{
    int angle, magnitude;

    if (abs(accel_x) >= MOTION_AXIS_MAX_NOISE)
        goto unreliable;

    if (abs(accel_y) >= MOTION_AXIS_MAX)
        goto unreliable;

    if (abs(accel_z) >= MOTION_AXIS_MAX)
        goto unreliable;

    magnitude = sqrt(accel_x * accel_x +
                     accel_y * accel_y +
                     accel_z * accel_z);

    if (magnitude >= MOTION_AXIS_MAX)
        goto unreliable;

    angle = (180 / M_PI) * atan((float)accel_y/(float)accel_z);

    if (angle < 0)
        angle += 180;

    if (accel_y < 0)
        angle += 180;

    if (accel_z > 0 && angle == 180)
        angle = 0;

    return angle;

unreliable:
    return ANGLE_ERROR;
}

int calculate_and_show_motion_angle(int x, int y, int z)
{
    int angle;
    float scale_x, scale_y, scale_z;

    angle = calculate_motion_angle(x, y, z);

    get_motion_scale(&scale_x, &scale_y, &scale_z);

    printf("%d", angle);

    if (print_all)
        printf(": X=%1.3f, Y=%1.3f, Z=%1.3f", scale_x*x, scale_y*y, scale_z*z);

    printf("\n");

    return angle;
}

int calculate_motion_angle_from_iio_device(void)
{
    int x, y, z;
    int rv;

    rv = get_motion_data(&x, &y, &z);

    DBGF("%s: X=%d, Y=%d, Z=%d\n", __func__, x, y, z);

    if (rv != 0) {
        printf("%s: fail to read motion sensor!\n", __func__);
        return -1;
    }

    return calculate_and_show_motion_angle(x, y, z);
}


int main(int argc, char *argv[])
{
    int val_x, val_y, val_z;
    int i, value, indata = 0;

    /* argument process */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {

                /* help function */
                case '-':
                    if (strcmp(argv[i], "--help") != 0)
                        break;
                case 'h':
                case 'H':
                    help();
                    return 0;
                /* get all (motion angle + motion sensor value) */
                case 'a':
                case 'A':
                    print_all = TRUE;
                    break;
                /* set motion sensor sampling frequency */
                case 'f':
                case 'F':
                    if (argv[i+1] == NULL) {
                       printf("Invalid data input.\n");
                       return -1;
                    }

                    value = atoi(argv[i+1]);

                    set_sampling_freq(value);
                    return 0;
                /* get raw motion sensor value */
                case 'g':
                case 'G':
                    mode = LOG_LEVEL_DEBUG;
                    get_motion_data(&val_x, &val_y, &val_z);
                    return 0;
                /* set number of iio device */
                case 'n':
                case 'N':
                    if (argv[i+1] == NULL) {
                       printf("Invalid data input.\n");
                       return -1;
                    }

                    value = atoi(argv[i+1]);

                    if (value < 0 || value > 2) {
                        printf("Invalid data input: %s\n", argv[i+1]);
                        return -1;
                    }
                    sprintf(iiodevice, "/sys/bus/iio/devices/iio:device%d", value);
                    DBGF("select device: %s\n",iiodevice);
                    break;
                /* show version */
                case 'v':
                case 'V':
                    printf("V%s %s %s\n", VERSION, BUILD_DATE, BUILD_TIME);
                    return 0;
                /* debug mode */
                case 'd':
                case 'D':
                    mode = LOG_LEVEL_DEBUG;
                    DBG("Enable debug output\n");
                    break;
                /* test mode */
                case 't':
                case 'T':
                    return 0;
                    break;
                /* unknown argument, ignore it. */
                default:
                    printf("warning: unknown argument: %s\n", argv[i]);
                    break;
            }
            continue;
        }
        indata++;
    }

    if (indata != 3) {
        calculate_motion_angle_from_iio_device();
        return 0;
    }

    val_x = atoi(argv[1]);
    val_y = atoi(argv[2]);
    val_z = atoi(argv[3]);

    DBGF("input data: X=%d, Y=%d, Z=%d\n", val_x, val_y, val_z);
    
    calculate_and_show_motion_angle(val_x, val_y, val_z);

    return 0;
}


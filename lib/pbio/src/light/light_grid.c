// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include <pbio/config.h>

#if PBIO_CONFIG_LIGHT_GRID

#include <stdbool.h>

#include <pbio/error.h>
#include <pbio/light_grid.h>
#include <pbio/util.h>

#include "animation.h"
#include "light_grid.h"

/**
 * Sets the pixel to a given brightness.
 *
 * @param [in]  light_grid  The light grid instance
 * @param [in]  row         Row index (0 to size-1)
 * @param [in]  col         Column index (0 to size-1)
 * @param [in]  brightness  Brightness (0 to 100)
 * @return                  ::PBIO_SUCCESS on success ::PBIO_ERROR_INVALID_ARG
 *                          if @p row or @p col is out of range or
 *                          implementation-specific error on failure.
 */
static pbio_error_t pbio_light_grid_set_pixel(pbio_light_grid_t *light_grid, uint8_t row, uint8_t col, uint8_t brightness) {
    uint8_t size = light_grid->size;
    if (row >= size || col >= size) {
        return PBIO_ERROR_INVALID_ARG;
    }

    // Rotate user input based on screen orientation
    switch (light_grid->up_side) {
        case PBIO_SIDE_TOP:
        case PBIO_SIDE_FRONT:
            break;
        case PBIO_SIDE_LEFT: {
            uint8_t _col = col;
            col = row;
            row = size - 1 - _col;
            break;
        }
        case PBIO_SIDE_BOTTOM:
        case PBIO_SIDE_BACK:
            col = size - 1 - col;
            row = size - 1 - row;
            break;
        case PBIO_SIDE_RIGHT: {
            uint8_t _col = col;
            col = size - 1 - row;
            row = _col;
            break;
        }
    }

    // Set the pixel brightness
    return light_grid->funcs->set_pixel(light_grid, row, col, brightness);
}


/**
 * Initializes the required fields in a ::pbio_light_grid_t.
 * @param [in]  light_grid  The struct to initialize.
 * @param [in]  size        The size of the light grid.
 * @param [in]  funcs       The instance-specific callback functions.
 */
void pbio_light_grid_init(pbio_light_grid_t *light_grid, uint8_t size, const pbio_light_grid_funcs_t *funcs) {
    light_grid->size = size;
    light_grid->funcs = funcs;
    pbio_light_animation_init(&light_grid->animation, NULL);
}

/**
 * Sets the light grid orientation
 * @param [in]  light_grid  The light grid.
 * @param [in]  up_side     Orientation of the light grid: which side is up.
 */
void pbio_light_grid_set_orientation(pbio_light_grid_t *light_grid, pbio_side_t up_side) {
    light_grid->up_side = up_side;
}


/**
 * Get the size of the light grid.
 *
 * Light grids are square, so this is equal to both the number of rows and the
 * number of columns in the grid.
 *
 * @param [in]  light_grid  The light grid instance.
 * @return                  The grid size.
 */
uint8_t pbio_light_grid_get_size(pbio_light_grid_t *light_grid) {
    return light_grid->size;
}

/**
 * Sets the pixels of all rows bitwise.
 *
 * Each bit in a byte sets a pixel in a row, where 1 means a pixel is on and 0
 * is off. The least significant bit is the right-most pixel. This is the same
 * format as used by the micro:bit.
 *
 * If an animation is running in the background, it will be stopped.
 *
 * @param [in]  light_grid  The light grid instance
 * @param [in]  rows        Array of size bytes. Each byte is one row, LSB right.
 * @return                  ::PBIO_SUCCESS on success or implementation-specific
 *                          error on failure.
 */
pbio_error_t pbio_light_grid_set_rows(pbio_light_grid_t *light_grid, const uint8_t *rows) {
    // Loop through all rows i, starting at row 0 at the top.
    uint8_t size = light_grid->size;
    for (uint8_t i = 0; i < size; i++) {
        // Loop through all columns j, starting at col 0 on the left.
        for (uint8_t j = 0; j < size; j++) {
            // The pixel is on if the bit is high.
            bool on = rows[i] & (1 << (size - 1 - j));
            // Set the pixel.
            pbio_error_t err = pbio_light_grid_set_pixel(light_grid, i, j, on * 100);
            if (err != PBIO_SUCCESS) {
                return err;
            }
        }
    }
    return PBIO_SUCCESS;
}

/**
 * User function that sets the pixel to a given brightness.
 *
 * If an animation is running in the background, it will be stopped. Out of
 * range index values are ignored and intentionally raise no error.
 *
 * @param [in]  light_grid  The light grid instance
 * @param [in]  row         Row index (0 to size-1)
 * @param [in]  col         Column index (0 to size-1)
 * @param [in]  brightness  Brightness (0 to 100)
 * @return                  ::PBIO_SUCCESS on success or
 *                          implementation-specific error on failure.
 */
pbio_error_t pbio_light_grid_set_pixel_user(pbio_light_grid_t *light_grid, uint8_t row, uint8_t col, uint8_t brightness) {

    pbio_light_grid_stop_animation(light_grid);

    uint8_t size = light_grid->size;
    if (row >= size || col >= size) {
        return PBIO_SUCCESS;
    }

    return pbio_light_grid_set_pixel(light_grid, row, col, brightness);
}

/**
 * Sets the pixel to a given brightness.
 *
 * If an animation is running in the background, it will be stopped.
 *
 * @param [in]  light_grid  The light grid instance
 * @param [in]  image       Buffer of brightness values (0 to 100)
 * @return                  ::PBIO_SUCCESS on success or implementation-specific
 *                          error on failure.
 */
pbio_error_t pbio_light_grid_set_image(pbio_light_grid_t *light_grid, const uint8_t *image) {
    uint8_t size = light_grid->size;
    for (uint8_t r = 0; r < size; r++) {
        for (uint8_t c = 0; c < size; c++) {
            pbio_error_t err = pbio_light_grid_set_pixel_user(light_grid, r, c, image[r * size + c]);
            if (err != PBIO_SUCCESS) {
                return err;
            }
        }
    }
    return PBIO_SUCCESS;
}

static uint32_t pbio_light_grid_animation_next(pbio_light_animation_t *animation) {
    pbio_light_grid_t *light_grid = PBIO_CONTAINER_OF(animation, pbio_light_grid_t, animation);

    // display the current cell
    uint8_t size = light_grid->size;
    const uint8_t *cell = light_grid->animation_cells + size * size * light_grid->current_cell;

    for (uint8_t r = 0; r < size; r++) {
        for (uint8_t c = 0; c < size; c++) {
            pbio_light_grid_set_pixel(light_grid, r, c, cell[r * size + c]);
        }
    }

    // move to the next cell
    if (++light_grid->current_cell >= light_grid->num_animation_cells) {
        light_grid->current_cell = 0;
    }

    return light_grid->interval;
}

/**
 * Starts animating the light grid in the background.
 *
 * If another animation is already running in the background, it will be stopped.
 *
 * @param [in]  light_grid  The light grid instance
 * @param [in]  cells       Array of size x size arrays of brightness values (0 to 100).
 * @param [in]  num_cells   Number of @p cells
 * @param [in]  interval    Time in milliseconds to wait between each cell.
 */
void pbio_light_grid_start_animation(pbio_light_grid_t *light_grid, const uint8_t *cells, uint8_t num_cells, uint16_t interval) {
    pbio_light_grid_stop_animation(light_grid);

    pbio_light_animation_init(&light_grid->animation, pbio_light_grid_animation_next);
    light_grid->animation_cells = cells;
    light_grid->num_animation_cells = num_cells;
    light_grid->interval = interval;
    light_grid->current_cell = 0;

    pbio_light_animation_start(&light_grid->animation);
}

/**
 * Stops the background animation.
 * @param [in]  light_grid  The light grid instance
 */
void pbio_light_grid_stop_animation(pbio_light_grid_t *light_grid) {
    if (pbio_light_animation_is_started(&light_grid->animation)) {
        pbio_light_animation_stop(&light_grid->animation);
    }
}

#endif // PBIO_CONFIG_LIGHT_GRID

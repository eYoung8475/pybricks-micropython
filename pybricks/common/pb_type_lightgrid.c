// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include "py/mpconfig.h"

#if PYBRICKS_PY_COMMON && PYBRICKS_PY_COMMON_LIGHT_GRID

#include <pbio/light_grid.h>

#include "py/mphal.h"
#include "py/obj.h"
#include "py/objstr.h"

#include <pybricks/geometry.h>
#include <pybricks/common.h>

#include <pybricks/util_pb/pb_error.h>
#include <pybricks/util_mp/pb_obj_helper.h>
#include <pybricks/util_mp/pb_kwarg_helper.h>


// pybricks._common.LightGrid class object
typedef struct _common_LightGrid_obj_t {
    mp_obj_base_t base;
    pbio_light_grid_t *light_grid;
    uint8_t *data;
    uint8_t frames;
} common_LightGrid_obj_t;

// Renews memory for a given number of frames
STATIC void common_LightGrid__renew(common_LightGrid_obj_t *self, uint8_t frames) {
    // Grid with/height
    size_t size = pbio_light_grid_get_size(self->light_grid);

    // Renew buffer for new number of frames
    self->data = m_renew(uint8_t, self->data, size * size * self->frames, size * size * frames);

    // Save new number of frames
    self->frames = frames;
}

// pybricks._common.LightGrid.char
STATIC mp_obj_t common_LightGrid_char(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        common_LightGrid_obj_t, self,
        PB_ARG_REQUIRED(char));

    // Assert that the input is a single character
    GET_STR_DATA_LEN(char_in, text, text_len);
    if (text_len != 1 || text[0] < 32 || text[0] > 126) {
        pb_assert(PBIO_ERROR_INVALID_ARG);
    }

    // Pick corresponding image and display it
    pb_assert(pbio_light_grid_set_rows(self->light_grid, pb_font_5x5[text[0] - 32]));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(common_LightGrid_char_obj, 1, common_LightGrid_char);

static void common_LightGrid_image__extract(mp_obj_t image_in, size_t size, uint8_t *data) {

    #if MICROPY_PY_BUILTINS_FLOAT
    // If image is a matrix, copy data from there
    if (mp_obj_is_type(image_in, &pb_type_Matrix)) {
        for (size_t r = 0; r < size; r++) {
            for (size_t c = 0; c < size; c++) {
                data[r * size + c] = (uint8_t)pb_type_Matrix_get_scalar(image_in, r, c);
            }
        }
        return;
    }
    #endif // MICROPY_PY_BUILTINS_FLOAT

    // Unpack the main list of rows and get the requested sizes
    mp_obj_t *row_objs, *scalar_objs;
    size_t m;
    mp_obj_get_array(image_in, &m, &row_objs);
    if (m != size) {
        pb_assert(PBIO_ERROR_INVALID_ARG);
    }

    // Iterate through each of the rows to get the scalars
    for (size_t r = 0; r < size; r++) {
        size_t n;
        mp_obj_get_array(row_objs[r], &n, &scalar_objs);
        if (n != size) {
            pb_assert(PBIO_ERROR_INVALID_ARG);
        }
        // Unpack the scalars
        for (size_t c = 0; c < size; c++) {
            data[r * size + c] = pb_obj_get_pct(scalar_objs[c]);
        }
    }
}

// pybricks._common.LightGrid.image
STATIC mp_obj_t common_LightGrid_image(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        common_LightGrid_obj_t, self,
        PB_ARG_REQUIRED(image));

    // Allocate and extract image data
    size_t size = pbio_light_grid_get_size(self->light_grid);
    common_LightGrid__renew(self, 1);
    common_LightGrid_image__extract(image_in, size, self->data);

    // Display the image
    pb_assert(pbio_light_grid_set_image(self->light_grid, self->data));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(common_LightGrid_image_obj, 1, common_LightGrid_image);

// pybricks._common.LightGrid.on
STATIC mp_obj_t common_LightGrid_on(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        common_LightGrid_obj_t, self,
        PB_ARG_DEFAULT_INT(brightness, 100));

    uint8_t size = pbio_light_grid_get_size(self->light_grid);

    mp_int_t brightness = pb_obj_get_pct(brightness_in);

    for (uint8_t i = 0; i < size; i++) {
        for (uint8_t j = 0; j < size; j++) {
            pb_assert(pbio_light_grid_set_pixel_user(self->light_grid, i, j, brightness));
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(common_LightGrid_on_obj, 1, common_LightGrid_on);

// pybricks._common.LightGrid.off
STATIC mp_obj_t common_LightGrid_off(mp_obj_t self_in) {
    common_LightGrid_obj_t *self = MP_OBJ_TO_PTR(self_in);

    uint8_t size = pbio_light_grid_get_size(self->light_grid);

    for (uint8_t i = 0; i < size; i++) {
        for (uint8_t j = 0; j < size; j++) {
            pb_assert(pbio_light_grid_set_pixel_user(self->light_grid, i, j, 0));
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(common_LightGrid_off_obj, common_LightGrid_off);

// pybricks._common.LightGrid.number
STATIC mp_obj_t common_LightGrid_number(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        common_LightGrid_obj_t, self,
        PB_ARG_REQUIRED(number));


    uint8_t size = pbio_light_grid_get_size(self->light_grid);

    // Currently numbers are only implemented for 5x5 grids
    if (size != 5) {
        pb_assert(PBIO_ERROR_NOT_IMPLEMENTED);
    }

    // Get the number
    mp_int_t number = pb_obj_get_int(number_in);

    // > 99 gets displayed as >
    if (number > 99) {
        pb_assert(pbio_light_grid_set_rows(self->light_grid, pb_font_5x5['>' - 32]));
        return mp_const_none;
    }

    // < -99 gets displayed as <
    if (number < -99) {
        pb_assert(pbio_light_grid_set_rows(self->light_grid, pb_font_5x5['<' - 32]));
        return mp_const_none;
    }

    // Remember sign but make value positive
    bool negative = number < 0;
    if (negative) {
        number = -number;
    }

    // Compose number as two digits
    uint8_t composite[5];
    for (uint8_t i = 0; i < 5; i++) {
        composite[i] = pb_digits_5x2[number / 10][i] << 3 | pb_digits_5x2[number % 10][i];
    }

    // Display the result
    pb_assert(pbio_light_grid_set_rows(self->light_grid, composite));

    // Display one faint dot in the middle to indicate negative
    if (negative) {
        pb_assert(pbio_light_grid_set_pixel_user(self->light_grid, 2, 2, 50));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(common_LightGrid_number_obj, 1, common_LightGrid_number);

// pybricks._common.LightGrid.pattern
STATIC mp_obj_t common_LightGrid_pattern(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        common_LightGrid_obj_t, self,
        PB_ARG_REQUIRED(images),
        PB_ARG_REQUIRED(interval));

    // Time between frames
    mp_int_t interval = pb_obj_get_int(interval_in);

    // Unpack the list of images
    mp_obj_t *image_objs;
    size_t n;
    mp_obj_get_array(images_in, &n, &image_objs);
    if (n > UINT8_MAX || n < 2) {
        pb_assert(PBIO_ERROR_INVALID_ARG);
    }

    // Allocate pattern data
    size_t size = pbio_light_grid_get_size(self->light_grid);
    common_LightGrid__renew(self, n);

    // Extract pattern data
    for (uint8_t i = 0; i < n; i++) {
        common_LightGrid_image__extract(image_objs[i], size, self->data + size * size * i);
    }

    // Activate the pattern
    pbio_light_grid_start_animation(self->light_grid, self->data, self->frames, interval);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(common_LightGrid_pattern_obj, 1, common_LightGrid_pattern);

// pybricks._common.LightGrid.pixel
STATIC mp_obj_t common_LightGrid_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        common_LightGrid_obj_t, self,
        PB_ARG_REQUIRED(row),
        PB_ARG_REQUIRED(column),
        PB_ARG_DEFAULT_INT(brightness, 100));

    // Set pixel at the given brightness
    pb_assert(pbio_light_grid_set_pixel_user(self->light_grid, mp_obj_get_int(row_in), mp_obj_get_int(column_in), pb_obj_get_pct(brightness_in)));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(common_LightGrid_pixel_obj, 1, common_LightGrid_pixel);

// pybricks._common.LightGrid.text
STATIC mp_obj_t common_LightGrid_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        common_LightGrid_obj_t, self,
        PB_ARG_REQUIRED(text),
        PB_ARG_DEFAULT_INT(on, 500),
        PB_ARG_DEFAULT_INT(off, 50));

    // Assert that the input is a single text
    GET_STR_DATA_LEN(text_in, text, text_len);

    // Make sure all characters are valid
    for (size_t i = 0; i < text_len; i++) {
        if (text[0] < 32 || text[0] > 126) {
            pb_assert(PBIO_ERROR_INVALID_ARG);
        }
    }

    mp_int_t on = pb_obj_get_int(on_in);
    mp_int_t off = pb_obj_get_int(off_in);

    // Display all characters one by one
    for (size_t i = 0; i < text_len; i++) {
        pb_assert(pbio_light_grid_set_rows(self->light_grid, pb_font_5x5[text[i] - 32]));
        mp_hal_delay_ms(on);

        // Some off time so we can see multiple of the same characters
        if (off > 0 || i == text_len - 1) {
            pb_assert(pbio_light_grid_set_rows(self->light_grid, pb_font_5x5[' ' - 32]));
            mp_hal_delay_ms(off);
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(common_LightGrid_text_obj, 1, common_LightGrid_text);

// dir(pybricks.builtins.LightGrid)
STATIC const mp_rom_map_elem_t common_LightGrid_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_char),   MP_ROM_PTR(&common_LightGrid_char_obj)   },
    { MP_ROM_QSTR(MP_QSTR_image),  MP_ROM_PTR(&common_LightGrid_image_obj)  },
    { MP_ROM_QSTR(MP_QSTR_on),     MP_ROM_PTR(&common_LightGrid_on_obj)     },
    { MP_ROM_QSTR(MP_QSTR_off),    MP_ROM_PTR(&common_LightGrid_off_obj)    },
    { MP_ROM_QSTR(MP_QSTR_number), MP_ROM_PTR(&common_LightGrid_number_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel),  MP_ROM_PTR(&common_LightGrid_pixel_obj)  },
    { MP_ROM_QSTR(MP_QSTR_pattern),MP_ROM_PTR(&common_LightGrid_pattern_obj)},
    { MP_ROM_QSTR(MP_QSTR_text),   MP_ROM_PTR(&common_LightGrid_text_obj)   },
};
STATIC MP_DEFINE_CONST_DICT(common_LightGrid_locals_dict, common_LightGrid_locals_dict_table);

// type(pybricks.builtins.LightGrid)
STATIC const mp_obj_type_t pb_type_LightGrid = {
    { &mp_type_type },
    .name = MP_QSTR_LightGrid,
    .locals_dict = (mp_obj_dict_t *)&common_LightGrid_locals_dict,
};

// pybricks._common.LightGrid.__init__
mp_obj_t pb_type_LightGrid_obj_new(pbio_light_grid_t *light_grid) {
    // Create new light instance
    common_LightGrid_obj_t *self = m_new_obj(common_LightGrid_obj_t);
    self->base.type = &pb_type_LightGrid;
    self->light_grid = light_grid;
    return self;
}

#endif // PYBRICKS_PY_COMMON && PYBRICKS_PY_COMMON_LIGHT_GRID

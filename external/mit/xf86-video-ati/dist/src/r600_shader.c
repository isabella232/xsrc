/*
 * Copyright 2008 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Author: Alex Deucher <alexander.deucher@amd.com>
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xf86.h"

#include "radeon.h"
#include "r600_shader.h"
#include "r600_reg.h"

/* solid vs --------------------------------------- */
int R600_solid_vs(RADEONChipFamily ChipSet, uint32_t* shader)
{
    int i = 0;

    /* 0 */
    shader[i++] = CF_DWORD0(ADDR(4));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(1),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_VTX),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));
    /* 1 */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(CF_POS0),
					  TYPE(SQ_EXPORT_POS),
					  RW_GPR(1),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(0));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_Z),
					       SRC_SEL_W(SQ_SEL_W),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(1),
					       END_OF_PROGRAM(0),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT_DONE),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(1));
    /* 2 - always export a param whether it's used or not */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(0),
					  TYPE(SQ_EXPORT_PARAM),
					  RW_GPR(0),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(0));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_Z),
					       SRC_SEL_W(SQ_SEL_W),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(0),
					       END_OF_PROGRAM(1),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT_DONE),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(0));
    /* 3 - padding */
    shader[i++] = 0x00000000;
    shader[i++] = 0x00000000;
    /* 4/5 */
    shader[i++] = VTX_DWORD0(VTX_INST(SQ_VTX_INST_FETCH),
			     FETCH_TYPE(SQ_VTX_FETCH_VERTEX_DATA),
			     FETCH_WHOLE_QUAD(0),
			     BUFFER_ID(0),
			     SRC_GPR(0),
			     SRC_REL(ABSOLUTE),
			     SRC_SEL_X(SQ_SEL_X),
			     MEGA_FETCH_COUNT(8));
    shader[i++] = VTX_DWORD1_GPR(DST_GPR(1),
				 DST_REL(0),
				 DST_SEL_X(SQ_SEL_X),
				 DST_SEL_Y(SQ_SEL_Y),
				 DST_SEL_Z(SQ_SEL_0),
				 DST_SEL_W(SQ_SEL_1),
				 USE_CONST_FIELDS(0),
				 DATA_FORMAT(FMT_32_32_FLOAT),
				 NUM_FORMAT_ALL(SQ_NUM_FORMAT_SCALED),
				 FORMAT_COMP_ALL(SQ_FORMAT_COMP_SIGNED),
				 SRF_MODE_ALL(SRF_MODE_ZERO_CLAMP_MINUS_ONE));
    shader[i++] = VTX_DWORD2(OFFSET(0),
			     ENDIAN_SWAP(ENDIAN_NONE),
			     CONST_BUF_NO_STRIDE(0),
			     MEGA_FETCH(1));
    shader[i++] = VTX_DWORD_PAD;

    return i;
}

/* solid ps --------------------------------------- */
int R600_solid_ps(RADEONChipFamily ChipSet, uint32_t* shader)
{
    int i = 0;

    /* 0 */
    shader[i++] = CF_ALU_DWORD0(ADDR(2),
				KCACHE_BANK0(0),
				KCACHE_BANK1(0),
				KCACHE_MODE0(SQ_CF_KCACHE_NOP));
    shader[i++] = CF_ALU_DWORD1(KCACHE_MODE1(SQ_CF_KCACHE_NOP),
				KCACHE_ADDR0(0),
				KCACHE_ADDR1(0),
				I_COUNT(4),
				USES_WATERFALL(0),
				CF_INST(SQ_CF_INST_ALU),
				WHOLE_QUAD_MODE(0),
				BARRIER(1));
    /* 1 */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(CF_PIXEL_MRT0),
					  TYPE(SQ_EXPORT_PIXEL),
					  RW_GPR(0),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(1));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_Z),
					       SRC_SEL_W(SQ_SEL_W),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(1),
					       END_OF_PROGRAM(1),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT_DONE),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(1));

    /* 2 */
    shader[i++] = ALU_DWORD0(SRC0_SEL(256),
			     SRC0_REL(ABSOLUTE),
			     SRC0_ELEM(ELEM_X),
			     SRC0_NEG(0),
			     SRC1_SEL(0),
			     SRC1_REL(ABSOLUTE),
			     SRC1_ELEM(ELEM_X),
			     SRC1_NEG(0),
			     INDEX_MODE(SQ_INDEX_AR_X),
			     PRED_SEL(SQ_PRED_SEL_OFF),
			     LAST(0));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
				 SRC0_ABS(0),
				 SRC1_ABS(0),
				 UPDATE_EXECUTE_MASK(0),
				 UPDATE_PRED(0),
				 WRITE_MASK(1),
				 FOG_MERGE(0),
				 OMOD(SQ_ALU_OMOD_OFF),
				 ALU_INST(SQ_OP2_INST_MOV),
				 BANK_SWIZZLE(SQ_ALU_VEC_012),
				 DST_GPR(0),
				 DST_REL(ABSOLUTE),
				 DST_ELEM(ELEM_X),
				 CLAMP(1));
    /* 3 */
    shader[i++] = ALU_DWORD0(SRC0_SEL(256),
			     SRC0_REL(ABSOLUTE),
			     SRC0_ELEM(ELEM_Y),
			     SRC0_NEG(0),
			     SRC1_SEL(0),
			     SRC1_REL(ABSOLUTE),
			     SRC1_ELEM(ELEM_Y),
			     SRC1_NEG(0),
			     INDEX_MODE(SQ_INDEX_AR_X),
			     PRED_SEL(SQ_PRED_SEL_OFF),
			     LAST(0));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
				 SRC0_ABS(0),
				 SRC1_ABS(0),
				 UPDATE_EXECUTE_MASK(0),
				 UPDATE_PRED(0),
				 WRITE_MASK(1),
				 FOG_MERGE(0),
				 OMOD(SQ_ALU_OMOD_OFF),
				 ALU_INST(SQ_OP2_INST_MOV),
				 BANK_SWIZZLE(SQ_ALU_VEC_012),
				 DST_GPR(0),
				 DST_REL(ABSOLUTE),
				 DST_ELEM(ELEM_Y),
				 CLAMP(1));
    /* 4 */
    shader[i++] = ALU_DWORD0(SRC0_SEL(256),
			     SRC0_REL(ABSOLUTE),
			     SRC0_ELEM(ELEM_Z),
			     SRC0_NEG(0),
			     SRC1_SEL(0),
			     SRC1_REL(ABSOLUTE),
			     SRC1_ELEM(ELEM_Z),
			     SRC1_NEG(0),
			     INDEX_MODE(SQ_INDEX_AR_X),
			     PRED_SEL(SQ_PRED_SEL_OFF),
			     LAST(0));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
				 SRC0_ABS(0),
				 SRC1_ABS(0),
				 UPDATE_EXECUTE_MASK(0),
				 UPDATE_PRED(0),
				 WRITE_MASK(1),
				 FOG_MERGE(0),
				 OMOD(SQ_ALU_OMOD_OFF),
				 ALU_INST(SQ_OP2_INST_MOV),
				 BANK_SWIZZLE(SQ_ALU_VEC_012),
				 DST_GPR(0),
				 DST_REL(ABSOLUTE),
				 DST_ELEM(ELEM_Z),
				 CLAMP(1));
    /* 5 */
    shader[i++] = ALU_DWORD0(SRC0_SEL(256),
			     SRC0_REL(ABSOLUTE),
			     SRC0_ELEM(ELEM_W),
			     SRC0_NEG(0),
			     SRC1_SEL(0),
			     SRC1_REL(ABSOLUTE),
			     SRC1_ELEM(ELEM_W),
			     SRC1_NEG(0),
			     INDEX_MODE(SQ_INDEX_AR_X),
			     PRED_SEL(SQ_PRED_SEL_OFF),
			     LAST(1));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
				 SRC0_ABS(0),
				 SRC1_ABS(0),
				 UPDATE_EXECUTE_MASK(0),
				 UPDATE_PRED(0),
				 WRITE_MASK(1),
				 FOG_MERGE(0),
				 OMOD(SQ_ALU_OMOD_OFF),
				 ALU_INST(SQ_OP2_INST_MOV),
				 BANK_SWIZZLE(SQ_ALU_VEC_012),
				 DST_GPR(0),
				 DST_REL(ABSOLUTE),
				 DST_ELEM(ELEM_W),
				 CLAMP(1));

    return i;
}

/* copy vs --------------------------------------- */
int R600_copy_vs(RADEONChipFamily ChipSet, uint32_t* shader)
{
    int i = 0;

    /* 0 */
    shader[i++] = CF_DWORD0(ADDR(4));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(2),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_VTX),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));
    /* 1 */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(CF_POS0),
					  TYPE(SQ_EXPORT_POS),
					  RW_GPR(1),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(0));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_Z),
					       SRC_SEL_W(SQ_SEL_W),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(0),
					       END_OF_PROGRAM(0),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT_DONE),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(1));
    /* 2 */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(0),
					  TYPE(SQ_EXPORT_PARAM),
					  RW_GPR(0),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(0));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_Z),
					       SRC_SEL_W(SQ_SEL_W),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(0),
					       END_OF_PROGRAM(1),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT_DONE),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(0));
    /* 3 */
    shader[i++] = 0x00000000;
    shader[i++] = 0x00000000;
    /* 4/5 */
    shader[i++] = VTX_DWORD0(VTX_INST(SQ_VTX_INST_FETCH),
			     FETCH_TYPE(SQ_VTX_FETCH_VERTEX_DATA),
			     FETCH_WHOLE_QUAD(0),
			     BUFFER_ID(0),
			     SRC_GPR(0),
			     SRC_REL(ABSOLUTE),
			     SRC_SEL_X(SQ_SEL_X),
			     MEGA_FETCH_COUNT(16));
    shader[i++] = VTX_DWORD1_GPR(DST_GPR(1),
				 DST_REL(0),
				 DST_SEL_X(SQ_SEL_X),
				 DST_SEL_Y(SQ_SEL_Y),
				 DST_SEL_Z(SQ_SEL_0),
				 DST_SEL_W(SQ_SEL_1),
				 USE_CONST_FIELDS(0),
				 DATA_FORMAT(FMT_32_32_FLOAT),
				 NUM_FORMAT_ALL(SQ_NUM_FORMAT_SCALED),
				 FORMAT_COMP_ALL(SQ_FORMAT_COMP_SIGNED),
				 SRF_MODE_ALL(SRF_MODE_ZERO_CLAMP_MINUS_ONE));
    shader[i++] = VTX_DWORD2(OFFSET(0),
			     ENDIAN_SWAP(ENDIAN_NONE),
			     CONST_BUF_NO_STRIDE(0),
			     MEGA_FETCH(1));
    shader[i++] = VTX_DWORD_PAD;
    /* 6/7 */
    shader[i++] = VTX_DWORD0(VTX_INST(SQ_VTX_INST_FETCH),
			     FETCH_TYPE(SQ_VTX_FETCH_VERTEX_DATA),
			     FETCH_WHOLE_QUAD(0),
			     BUFFER_ID(0),
			     SRC_GPR(0),
			     SRC_REL(ABSOLUTE),
			     SRC_SEL_X(SQ_SEL_X),
			     MEGA_FETCH_COUNT(8));
    shader[i++] = VTX_DWORD1_GPR(DST_GPR(0),
				 DST_REL(0),
				 DST_SEL_X(SQ_SEL_X),
				 DST_SEL_Y(SQ_SEL_Y),
				 DST_SEL_Z(SQ_SEL_0),
				 DST_SEL_W(SQ_SEL_1),
				 USE_CONST_FIELDS(0),
				 DATA_FORMAT(FMT_32_32_FLOAT),
				 NUM_FORMAT_ALL(SQ_NUM_FORMAT_SCALED),
				 FORMAT_COMP_ALL(SQ_FORMAT_COMP_SIGNED),
				 SRF_MODE_ALL(SRF_MODE_ZERO_CLAMP_MINUS_ONE));
    shader[i++] = VTX_DWORD2(OFFSET(8),
			     ENDIAN_SWAP(ENDIAN_NONE),
			     CONST_BUF_NO_STRIDE(0),
			     MEGA_FETCH(0));
    shader[i++] = VTX_DWORD_PAD;

    return i;
}

/* copy ps --------------------------------------- */
int R600_copy_ps(RADEONChipFamily ChipSet, uint32_t* shader)
{
    int i=0;

    /* CF INST 0 */
    shader[i++] = CF_DWORD0(ADDR(2));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(1),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_TEX),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));
    /* CF INST 1 */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(CF_PIXEL_MRT0),
					  TYPE(SQ_EXPORT_PIXEL),
					  RW_GPR(0),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(1));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_Z),
					       SRC_SEL_W(SQ_SEL_W),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(1),
					       END_OF_PROGRAM(1),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT_DONE),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(1));
    /* TEX INST 0 */
    shader[i++] = TEX_DWORD0(TEX_INST(SQ_TEX_INST_SAMPLE),
			     BC_FRAC_MODE(0),
			     FETCH_WHOLE_QUAD(0),
			     RESOURCE_ID(0),
			     SRC_GPR(0),
			     SRC_REL(ABSOLUTE),
			     R7xx_ALT_CONST(0));
    shader[i++] = TEX_DWORD1(DST_GPR(0),
			     DST_REL(ABSOLUTE),
			     DST_SEL_X(SQ_SEL_X), /* R */
			     DST_SEL_Y(SQ_SEL_Y), /* G */
			     DST_SEL_Z(SQ_SEL_Z), /* B */
			     DST_SEL_W(SQ_SEL_W), /* A */
			     LOD_BIAS(0),
			     COORD_TYPE_X(TEX_UNNORMALIZED),
			     COORD_TYPE_Y(TEX_UNNORMALIZED),
			     COORD_TYPE_Z(TEX_UNNORMALIZED),
			     COORD_TYPE_W(TEX_UNNORMALIZED));
    shader[i++] = TEX_DWORD2(OFFSET_X(0),
			     OFFSET_Y(0),
			     OFFSET_Z(0),
			     SAMPLER_ID(0),
			     SRC_SEL_X(SQ_SEL_X),
			     SRC_SEL_Y(SQ_SEL_Y),
			     SRC_SEL_Z(SQ_SEL_0),
			     SRC_SEL_W(SQ_SEL_1));
    shader[i++] = TEX_DWORD_PAD;

    return i;
}

/*
 * ; xv vertex shader
 * 00 VTX: ADDR(4) CNT(2) 
 *       0  VFETCH R1.xy01, R0.x, fc0  MEGA(16) FORMAT(32_32_FLOAT) 
 *          FORMAT_COMP(SIGNED) 
 *       1  VFETCH R0.xy01, R0.x, fc0  MINI(8) OFFSET(8) FORMAT(32_32_FLOAT) 
 *          FORMAT_COMP(SIGNED) 
 * 01 EXP_DONE: POS0, R1
 * 02 EXP_DONE: PARAM0, R0  NO_BARRIER 
 * END_OF_PROGRAM
 */
int R600_xv_vs(RADEONChipFamily ChipSet, uint32_t* shader)
{
    int i = 0;

    /* 0 */
    shader[i++] = CF_DWORD0(ADDR(6));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
                            CF_CONST(0),
                            COND(SQ_CF_COND_ACTIVE),
                            I_COUNT(2),
                            CALL_COUNT(0),
                            END_OF_PROGRAM(0),
                            VALID_PIXEL_MODE(0),
                            CF_INST(SQ_CF_INST_VTX),
                            WHOLE_QUAD_MODE(0),
                            BARRIER(1));

    /* 1 - ALU */
    shader[i++] = CF_ALU_DWORD0(ADDR(4),
				KCACHE_BANK0(0),
				KCACHE_BANK1(0),
				KCACHE_MODE0(SQ_CF_KCACHE_NOP));
    shader[i++] = CF_ALU_DWORD1(KCACHE_MODE1(SQ_CF_KCACHE_NOP),
				KCACHE_ADDR0(0),
				KCACHE_ADDR1(0),
				I_COUNT(2),
				USES_WATERFALL(0),
				CF_INST(SQ_CF_INST_ALU),
				WHOLE_QUAD_MODE(0),
				BARRIER(1));

    /* 2 */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(CF_POS0),
                                          TYPE(SQ_EXPORT_POS),
                                          RW_GPR(1),
                                          RW_REL(ABSOLUTE),
                                          INDEX_GPR(0),
                                          ELEM_SIZE(3));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
                                               SRC_SEL_Y(SQ_SEL_Y),
                                               SRC_SEL_Z(SQ_SEL_Z),
                                               SRC_SEL_W(SQ_SEL_W),
                                               R6xx_ELEM_LOOP(0),
                                               BURST_COUNT(1),
                                               END_OF_PROGRAM(0),
                                               VALID_PIXEL_MODE(0),
                                               CF_INST(SQ_CF_INST_EXPORT_DONE),
                                               WHOLE_QUAD_MODE(0),
                                               BARRIER(1));
    /* 3 */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(0),
                                          TYPE(SQ_EXPORT_PARAM),
                                          RW_GPR(0),
                                          RW_REL(ABSOLUTE),
                                          INDEX_GPR(0),
                                          ELEM_SIZE(3));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
                                               SRC_SEL_Y(SQ_SEL_Y),
                                               SRC_SEL_Z(SQ_SEL_Z),
                                               SRC_SEL_W(SQ_SEL_W),
                                               R6xx_ELEM_LOOP(0),
                                               BURST_COUNT(1),
                                               END_OF_PROGRAM(1),
                                               VALID_PIXEL_MODE(0),
                                               CF_INST(SQ_CF_INST_EXPORT_DONE),
                                               WHOLE_QUAD_MODE(0),
                                               BARRIER(0));


    /* 4 texX / w */
    shader[i++] = ALU_DWORD0(SRC0_SEL(0),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(256),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_AR_X),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
                                 SRC0_ABS(0),
                                 SRC1_ABS(0),
                                 UPDATE_EXECUTE_MASK(0),
                                 UPDATE_PRED(0),
                                 WRITE_MASK(1),
                                 FOG_MERGE(0),
                                 OMOD(SQ_ALU_OMOD_OFF),
                                 ALU_INST(SQ_OP2_INST_MUL),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_X),
                                 CLAMP(0));

    /* 5 texY / h */
    shader[i++] = ALU_DWORD0(SRC0_SEL(0),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Y),
                             SRC0_NEG(0),
                             SRC1_SEL(256),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Y),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_AR_X),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
                                 SRC0_ABS(0),
                                 SRC1_ABS(0),
                                 UPDATE_EXECUTE_MASK(0),
                                 UPDATE_PRED(0),
                                 WRITE_MASK(1),
                                 FOG_MERGE(0),
                                 OMOD(SQ_ALU_OMOD_OFF),
                                 ALU_INST(SQ_OP2_INST_MUL),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Y),
                                 CLAMP(0));

    /* 6/7 */
    shader[i++] = VTX_DWORD0(VTX_INST(SQ_VTX_INST_FETCH),
                             FETCH_TYPE(SQ_VTX_FETCH_VERTEX_DATA),
                             FETCH_WHOLE_QUAD(0),
                             BUFFER_ID(0),
                             SRC_GPR(0),
                             SRC_REL(ABSOLUTE),
                             SRC_SEL_X(SQ_SEL_X),
                             MEGA_FETCH_COUNT(16));
    shader[i++] = VTX_DWORD1_GPR(DST_GPR(1),
                                 DST_REL(ABSOLUTE),
                                 DST_SEL_X(SQ_SEL_X),
                                 DST_SEL_Y(SQ_SEL_Y),
                                 DST_SEL_Z(SQ_SEL_0),
                                 DST_SEL_W(SQ_SEL_1),
                                 USE_CONST_FIELDS(0),
                                 DATA_FORMAT(FMT_32_32_FLOAT),
                                 NUM_FORMAT_ALL(SQ_NUM_FORMAT_SCALED),
                                 FORMAT_COMP_ALL(SQ_FORMAT_COMP_SIGNED),
                                 SRF_MODE_ALL(SRF_MODE_ZERO_CLAMP_MINUS_ONE));
    shader[i++] = VTX_DWORD2(OFFSET(0),
                             ENDIAN_SWAP(ENDIAN_NONE),
                             CONST_BUF_NO_STRIDE(0),
                             MEGA_FETCH(1));
    shader[i++] = VTX_DWORD_PAD;
    /* 8/9 */
    shader[i++] = VTX_DWORD0(VTX_INST(SQ_VTX_INST_FETCH),
                             FETCH_TYPE(SQ_VTX_FETCH_VERTEX_DATA),
                             FETCH_WHOLE_QUAD(0),
                             BUFFER_ID(0),
                             SRC_GPR(0),
                             SRC_REL(ABSOLUTE),
                             SRC_SEL_X(SQ_SEL_X),
                             MEGA_FETCH_COUNT(8));
    shader[i++] = VTX_DWORD1_GPR(DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_SEL_X(SQ_SEL_X),
                                 DST_SEL_Y(SQ_SEL_Y),
                                 DST_SEL_Z(SQ_SEL_0),
                                 DST_SEL_W(SQ_SEL_1),
                                 USE_CONST_FIELDS(0),
                                 DATA_FORMAT(FMT_32_32_FLOAT),
                                 NUM_FORMAT_ALL(SQ_NUM_FORMAT_SCALED),
                                 FORMAT_COMP_ALL(SQ_FORMAT_COMP_SIGNED),
                                 SRF_MODE_ALL(SRF_MODE_ZERO_CLAMP_MINUS_ONE));
    shader[i++] = VTX_DWORD2(OFFSET(8),
                             ENDIAN_SWAP(ENDIAN_NONE),
                             CONST_BUF_NO_STRIDE(0),
                             MEGA_FETCH(0));
    shader[i++] = VTX_DWORD_PAD;

    return i;
}

int R600_xv_ps(RADEONChipFamily ChipSet, uint32_t* shader)
{
    int i = 0;

    /* 0 */
    shader[i++] = CF_DWORD0(ADDR(16));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
                            CF_CONST(0),
                            COND(SQ_CF_COND_BOOL),
                            I_COUNT(0),
                            CALL_COUNT(0),
                            END_OF_PROGRAM(0),
                            VALID_PIXEL_MODE(0),
                            CF_INST(SQ_CF_INST_CALL),
                            WHOLE_QUAD_MODE(0),
                            BARRIER(0));
    /* 1 */
    shader[i++] = CF_DWORD0(ADDR(24));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
                            CF_CONST(0),
                            COND(SQ_CF_COND_NOT_BOOL),
                            I_COUNT(0),
                            CALL_COUNT(0),
                            END_OF_PROGRAM(0),
                            VALID_PIXEL_MODE(0),
                            CF_INST(SQ_CF_INST_CALL),
                            WHOLE_QUAD_MODE(0),
                            BARRIER(0));
    /* 2 */
    shader[i++] = CF_ALU_DWORD0(ADDR(4),
                                KCACHE_BANK0(0),
                                KCACHE_BANK1(0),
                                KCACHE_MODE0(SQ_CF_KCACHE_NOP));
    shader[i++] = CF_ALU_DWORD1(KCACHE_MODE1(SQ_CF_KCACHE_NOP),
                                KCACHE_ADDR0(0),
                                KCACHE_ADDR1(0),
                                I_COUNT(12),
                                USES_WATERFALL(0),
                                CF_INST(SQ_CF_INST_ALU),
                                WHOLE_QUAD_MODE(0),
                                BARRIER(1));
    /* 3 */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(CF_PIXEL_MRT0),
                                          TYPE(SQ_EXPORT_PIXEL),
                                          RW_GPR(2),
                                          RW_REL(ABSOLUTE),
                                          INDEX_GPR(0),
                                          ELEM_SIZE(3));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
                                               SRC_SEL_Y(SQ_SEL_Y),
                                               SRC_SEL_Z(SQ_SEL_Z),
                                               SRC_SEL_W(SQ_SEL_W),
                                               R6xx_ELEM_LOOP(0),
                                               BURST_COUNT(1),
                                               END_OF_PROGRAM(1),
                                               VALID_PIXEL_MODE(0),
                                               CF_INST(SQ_CF_INST_EXPORT_DONE),
                                               WHOLE_QUAD_MODE(0),
                                               BARRIER(1));
    /* 4,5,6,7 */
    /* r2.x = MAD(c0.w, r1.x, c0.x) */
    shader[i++] = ALU_DWORD0(SRC0_SEL(256),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_W),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(256),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_X),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(2),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_X),
                                 CLAMP(0));
    /* r2.y = MAD(c0.w, r1.x, c0.y) */
    shader[i++] = ALU_DWORD0(SRC0_SEL(256),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_W),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(256),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Y),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(2),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Y),
                                 CLAMP(0));
    /* r2.z = MAD(c0.w, r1.x, c0.z) */
    shader[i++] = ALU_DWORD0(SRC0_SEL(256),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_W),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(256),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Z),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(2),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Z),
                                 CLAMP(0));
    /* r2.w = MAD(0, 0, 1) */
    shader[i++] = ALU_DWORD0(SRC0_SEL(SQ_ALU_SRC_0),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(SQ_ALU_SRC_0),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(SQ_ALU_SRC_1),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_X),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(2),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_W),
                                 CLAMP(0));

    /* 8,9,10,11 */
    /* r2.x = MAD(c1.x, r1.y, pv.x) */
    shader[i++] = ALU_DWORD0(SRC0_SEL(257),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Y),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(SQ_ALU_SRC_PV),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_X),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(2),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_X),
                                 CLAMP(0));
    /* r2.y = MAD(c1.y, r1.y, pv.y) */
    shader[i++] = ALU_DWORD0(SRC0_SEL(257),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Y),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Y),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(SQ_ALU_SRC_PV),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Y),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(2),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Y),
                                 CLAMP(0));
    /* r2.z = MAD(c1.z, r1.y, pv.z) */
    shader[i++] = ALU_DWORD0(SRC0_SEL(257),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Z),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Y),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(SQ_ALU_SRC_PV),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Z),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(2),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Z),
                                 CLAMP(0));
    /* r2.w = MAD(0, 0, 1) */
    shader[i++] = ALU_DWORD0(SRC0_SEL(SQ_ALU_SRC_0),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(SQ_ALU_SRC_0),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(SQ_ALU_SRC_1),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_W),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(2),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_W),
                                 CLAMP(0));
    /* 12,13,14,15 */
    /* r2.x = MAD(c2.x, r1.z, pv.x) */
    shader[i++] = ALU_DWORD0(SRC0_SEL(258),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Z),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(SQ_ALU_SRC_PV),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_X),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(2),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_X),
                                 CLAMP(1));
    /* r2.y = MAD(c2.y, r1.z, pv.y) */
    shader[i++] = ALU_DWORD0(SRC0_SEL(258),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Y),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Z),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(SQ_ALU_SRC_PV),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Y),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(2),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Y),
                                 CLAMP(1));
    /* r2.z = MAD(c2.z, r1.z, pv.z) */
    shader[i++] = ALU_DWORD0(SRC0_SEL(258),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Z),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Z),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(SQ_ALU_SRC_PV),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Z),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(2),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Z),
                                 CLAMP(1));
    /* r2.w = MAD(0, 0, 1) */
    shader[i++] = ALU_DWORD0(SRC0_SEL(SQ_ALU_SRC_0),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(SQ_ALU_SRC_0),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(SQ_ALU_SRC_1),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_X),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(2),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_W),
                                 CLAMP(1));

    /* 16 */
    shader[i++] = CF_DWORD0(ADDR(18));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
                            CF_CONST(0),
                            COND(SQ_CF_COND_ACTIVE),
                            I_COUNT(3),
                            CALL_COUNT(0),
                            END_OF_PROGRAM(0),
                            VALID_PIXEL_MODE(0),
                            CF_INST(SQ_CF_INST_TEX),
                            WHOLE_QUAD_MODE(0),
                            BARRIER(1));
    /* 17 */
    shader[i++] = CF_DWORD0(ADDR(0));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(0),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_RETURN),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));
    /* 18/19 */
    shader[i++] = TEX_DWORD0(TEX_INST(SQ_TEX_INST_SAMPLE),
                             BC_FRAC_MODE(0),
                             FETCH_WHOLE_QUAD(0),
                             RESOURCE_ID(0),
                             SRC_GPR(0),
                             SRC_REL(ABSOLUTE),
                             R7xx_ALT_CONST(0));
    shader[i++] = TEX_DWORD1(DST_GPR(1),
                             DST_REL(ABSOLUTE),
                             DST_SEL_X(SQ_SEL_X),
                             DST_SEL_Y(SQ_SEL_MASK),
                             DST_SEL_Z(SQ_SEL_MASK),
                             DST_SEL_W(SQ_SEL_1),
                             LOD_BIAS(0),
                             COORD_TYPE_X(TEX_NORMALIZED),
                             COORD_TYPE_Y(TEX_NORMALIZED),
                             COORD_TYPE_Z(TEX_NORMALIZED),
                             COORD_TYPE_W(TEX_NORMALIZED));
    shader[i++] = TEX_DWORD2(OFFSET_X(0),
                             OFFSET_Y(0),
                             OFFSET_Z(0),
                             SAMPLER_ID(0),
                             SRC_SEL_X(SQ_SEL_X),
                             SRC_SEL_Y(SQ_SEL_Y),
                             SRC_SEL_Z(SQ_SEL_0),
                             SRC_SEL_W(SQ_SEL_1));
    shader[i++] = TEX_DWORD_PAD;
    /* 20/21 */
    shader[i++] = TEX_DWORD0(TEX_INST(SQ_TEX_INST_SAMPLE),
                             BC_FRAC_MODE(0),
                             FETCH_WHOLE_QUAD(0),
                             RESOURCE_ID(1),
                             SRC_GPR(0),
                             SRC_REL(ABSOLUTE),
                             R7xx_ALT_CONST(0));
    shader[i++] = TEX_DWORD1(DST_GPR(1),
                             DST_REL(ABSOLUTE),
                             DST_SEL_X(SQ_SEL_MASK),
                             DST_SEL_Y(SQ_SEL_MASK),
                             DST_SEL_Z(SQ_SEL_X),
                             DST_SEL_W(SQ_SEL_MASK),
                             LOD_BIAS(0),
                             COORD_TYPE_X(TEX_NORMALIZED),
                             COORD_TYPE_Y(TEX_NORMALIZED),
                             COORD_TYPE_Z(TEX_NORMALIZED),
                             COORD_TYPE_W(TEX_NORMALIZED));
    shader[i++] = TEX_DWORD2(OFFSET_X(0),
                             OFFSET_Y(0),
                             OFFSET_Z(0),
                             SAMPLER_ID(1),
                             SRC_SEL_X(SQ_SEL_X),
                             SRC_SEL_Y(SQ_SEL_Y),
                             SRC_SEL_Z(SQ_SEL_0),
                             SRC_SEL_W(SQ_SEL_1));
    shader[i++] = TEX_DWORD_PAD;
    /* 22/23 */
    shader[i++] = TEX_DWORD0(TEX_INST(SQ_TEX_INST_SAMPLE),
                             BC_FRAC_MODE(0),
                             FETCH_WHOLE_QUAD(0),
                             RESOURCE_ID(2),
                             SRC_GPR(0),
                             SRC_REL(ABSOLUTE),
                             R7xx_ALT_CONST(0));
    shader[i++] = TEX_DWORD1(DST_GPR(1),
                             DST_REL(ABSOLUTE),
                             DST_SEL_X(SQ_SEL_MASK),
                             DST_SEL_Y(SQ_SEL_X),
                             DST_SEL_Z(SQ_SEL_MASK),
                             DST_SEL_W(SQ_SEL_MASK),
                             LOD_BIAS(0),
                             COORD_TYPE_X(TEX_NORMALIZED),
                             COORD_TYPE_Y(TEX_NORMALIZED),
                             COORD_TYPE_Z(TEX_NORMALIZED),
                             COORD_TYPE_W(TEX_NORMALIZED));
    shader[i++] = TEX_DWORD2(OFFSET_X(0),
                             OFFSET_Y(0),
                             OFFSET_Z(0),
                             SAMPLER_ID(2),
                             SRC_SEL_X(SQ_SEL_X),
                             SRC_SEL_Y(SQ_SEL_Y),
                             SRC_SEL_Z(SQ_SEL_0),
                             SRC_SEL_W(SQ_SEL_1));
    shader[i++] = TEX_DWORD_PAD;
    /* 24 */
    shader[i++] = CF_DWORD0(ADDR(26));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
                            CF_CONST(0),
                            COND(SQ_CF_COND_ACTIVE),
                            I_COUNT(2),
                            CALL_COUNT(0),
                            END_OF_PROGRAM(0),
                            VALID_PIXEL_MODE(0),
                            CF_INST(SQ_CF_INST_TEX),
                            WHOLE_QUAD_MODE(0),
                            BARRIER(1));
    /* 25 */
    shader[i++] = CF_DWORD0(ADDR(0));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(0),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_RETURN),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));
    /* 26/27 */
    shader[i++] = TEX_DWORD0(TEX_INST(SQ_TEX_INST_SAMPLE),
                             BC_FRAC_MODE(0),
                             FETCH_WHOLE_QUAD(0),
                             RESOURCE_ID(0),
                             SRC_GPR(0),
                             SRC_REL(ABSOLUTE),
                             R7xx_ALT_CONST(0));
    shader[i++] = TEX_DWORD1(DST_GPR(1),
                             DST_REL(ABSOLUTE),
                             DST_SEL_X(SQ_SEL_X),
                             DST_SEL_Y(SQ_SEL_MASK),
                             DST_SEL_Z(SQ_SEL_MASK),
                             DST_SEL_W(SQ_SEL_1),
                             LOD_BIAS(0),
                             COORD_TYPE_X(TEX_NORMALIZED),
                             COORD_TYPE_Y(TEX_NORMALIZED),
                             COORD_TYPE_Z(TEX_NORMALIZED),
                             COORD_TYPE_W(TEX_NORMALIZED));
    shader[i++] = TEX_DWORD2(OFFSET_X(0),
                             OFFSET_Y(0),
                             OFFSET_Z(0),
                             SAMPLER_ID(0),
                             SRC_SEL_X(SQ_SEL_X),
                             SRC_SEL_Y(SQ_SEL_Y),
                             SRC_SEL_Z(SQ_SEL_0),
                             SRC_SEL_W(SQ_SEL_1));
    shader[i++] = TEX_DWORD_PAD;
    /* 28/29 */
    shader[i++] = TEX_DWORD0(TEX_INST(SQ_TEX_INST_SAMPLE),
                             BC_FRAC_MODE(0),
                             FETCH_WHOLE_QUAD(0),
                             RESOURCE_ID(1),
                             SRC_GPR(0),
                             SRC_REL(ABSOLUTE),
                             R7xx_ALT_CONST(0));
    shader[i++] = TEX_DWORD1(DST_GPR(1),
                             DST_REL(ABSOLUTE),
                             DST_SEL_X(SQ_SEL_MASK),
                             DST_SEL_Y(SQ_SEL_X),
                             DST_SEL_Z(SQ_SEL_Y),
                             DST_SEL_W(SQ_SEL_MASK),
                             LOD_BIAS(0),
                             COORD_TYPE_X(TEX_NORMALIZED),
                             COORD_TYPE_Y(TEX_NORMALIZED),
                             COORD_TYPE_Z(TEX_NORMALIZED),
                             COORD_TYPE_W(TEX_NORMALIZED));
    shader[i++] = TEX_DWORD2(OFFSET_X(0),
                             OFFSET_Y(0),
                             OFFSET_Z(0),
                             SAMPLER_ID(1),
                             SRC_SEL_X(SQ_SEL_X),
                             SRC_SEL_Y(SQ_SEL_Y),
                             SRC_SEL_Z(SQ_SEL_0),
                             SRC_SEL_W(SQ_SEL_1));
    shader[i++] = TEX_DWORD_PAD;

    return i;
}

/* comp vs --------------------------------------- */
int R600_comp_vs(RADEONChipFamily ChipSet, uint32_t* shader)
{
    int i = 0;

    /* 0 */
    shader[i++] = CF_DWORD0(ADDR(3));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
                            CF_CONST(0),
                            COND(SQ_CF_COND_BOOL),
                            I_COUNT(0),
                            CALL_COUNT(0),
                            END_OF_PROGRAM(0),
                            VALID_PIXEL_MODE(0),
                            CF_INST(SQ_CF_INST_CALL),
                            WHOLE_QUAD_MODE(0),
                            BARRIER(0));
    /* 1 */
    shader[i++] = CF_DWORD0(ADDR(9));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
                            CF_CONST(0),
                            COND(SQ_CF_COND_NOT_BOOL),
                            I_COUNT(0),
                            CALL_COUNT(0),
                            END_OF_PROGRAM(0),
                            VALID_PIXEL_MODE(0),
                            CF_INST(SQ_CF_INST_CALL),
                            WHOLE_QUAD_MODE(0),
                            BARRIER(0));
    /* 2 */
    shader[i++] = CF_DWORD0(ADDR(0));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
                            CF_CONST(0),
                            COND(SQ_CF_COND_ACTIVE),
                            I_COUNT(0),
                            CALL_COUNT(0),
                            END_OF_PROGRAM(1),
                            VALID_PIXEL_MODE(0),
                            CF_INST(SQ_CF_INST_NOP),
                            WHOLE_QUAD_MODE(0),
                            BARRIER(1));
    /* 3 - mask sub */
    shader[i++] = CF_DWORD0(ADDR(32));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(3),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_VTX),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));

    /* 4 - ALU */
    shader[i++] = CF_ALU_DWORD0(ADDR(14),
				KCACHE_BANK0(0),
				KCACHE_BANK1(0),
				KCACHE_MODE0(SQ_CF_KCACHE_NOP));
    shader[i++] = CF_ALU_DWORD1(KCACHE_MODE1(SQ_CF_KCACHE_NOP),
				KCACHE_ADDR0(0),
				KCACHE_ADDR1(0),
				I_COUNT(12),
				USES_WATERFALL(0),
				CF_INST(SQ_CF_INST_ALU),
				WHOLE_QUAD_MODE(0),
				BARRIER(1));

    /* 5 - dst */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(CF_POS0),
					  TYPE(SQ_EXPORT_POS),
					  RW_GPR(2),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(0));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_0),
					       SRC_SEL_W(SQ_SEL_1),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(1),
					       END_OF_PROGRAM(0),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT_DONE),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(1));
    /* 6 - src */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(0),
					  TYPE(SQ_EXPORT_PARAM),
					  RW_GPR(1),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(0));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_0),
					       SRC_SEL_W(SQ_SEL_1),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(1),
					       END_OF_PROGRAM(0),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(0));
    /* 7 - mask */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(1),
					  TYPE(SQ_EXPORT_PARAM),
					  RW_GPR(0),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(0));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_0),
					       SRC_SEL_W(SQ_SEL_1),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(1),
					       END_OF_PROGRAM(0),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT_DONE),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(0));
    /* 8 */
    shader[i++] = CF_DWORD0(ADDR(0));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(0),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_RETURN),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));
    /* 9 - non-mask sub */
    shader[i++] = CF_DWORD0(ADDR(38));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(2),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_VTX),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));

    /* 10 - ALU */
    shader[i++] = CF_ALU_DWORD0(ADDR(26),
				KCACHE_BANK0(0),
				KCACHE_BANK1(0),
				KCACHE_MODE0(SQ_CF_KCACHE_NOP));
    shader[i++] = CF_ALU_DWORD1(KCACHE_MODE1(SQ_CF_KCACHE_NOP),
				KCACHE_ADDR0(0),
				KCACHE_ADDR1(0),
				I_COUNT(6),
				USES_WATERFALL(0),
				CF_INST(SQ_CF_INST_ALU),
				WHOLE_QUAD_MODE(0),
				BARRIER(1));

    /* 11 - dst */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(CF_POS0),
					  TYPE(SQ_EXPORT_POS),
					  RW_GPR(1),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(0));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_0),
					       SRC_SEL_W(SQ_SEL_1),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(0),
					       END_OF_PROGRAM(0),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT_DONE),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(1));
    /* 12 - src */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(0),
					  TYPE(SQ_EXPORT_PARAM),
					  RW_GPR(0),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(0));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_0),
					       SRC_SEL_W(SQ_SEL_1),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(0),
					       END_OF_PROGRAM(0),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT_DONE),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(0));
    /* 13 */
    shader[i++] = CF_DWORD0(ADDR(0));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(0),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_RETURN),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));


    /* 14 srcX MAD - mask */
    shader[i++] = ALU_DWORD0(SRC0_SEL(256),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Y),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Y),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(256),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Z),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(1),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Z),
                                 CLAMP(0));
    /* 15 srcY MAD */
    shader[i++] = ALU_DWORD0(SRC0_SEL(257),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Y),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Y),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(257),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Z),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(1),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_W),
                                 CLAMP(0));

    /* 16 srcX MAD */
    shader[i++] = ALU_DWORD0(SRC0_SEL(256),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(1),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Z),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(1),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_X),
                                 CLAMP(0));
    /* 17 srcY MAD */
    shader[i++] = ALU_DWORD0(SRC0_SEL(257),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(1),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(1),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_W),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(1),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Y),
                                 CLAMP(0));

    /* 18 maskX MAD */
    shader[i++] = ALU_DWORD0(SRC0_SEL(258),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Y),
                             SRC0_NEG(0),
                             SRC1_SEL(0),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Y),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(258),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Z),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Z),
                                 CLAMP(0));

    /* 19 maskY MAD */
    shader[i++] = ALU_DWORD0(SRC0_SEL(259),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Y),
                             SRC0_NEG(0),
                             SRC1_SEL(0),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Y),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(259),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Z),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_W),
                                 CLAMP(0));

    /* 20 srcX MAD */
    shader[i++] = ALU_DWORD0(SRC0_SEL(258),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(0),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(0),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Z),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_X),
                                 CLAMP(0));
    /* 21 srcY MAD */
    shader[i++] = ALU_DWORD0(SRC0_SEL(259),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(0),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(0),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_W),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Y),
                                 CLAMP(0));

    /* 22 srcX / w */
    shader[i++] = ALU_DWORD0(SRC0_SEL(1),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(256),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_W),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_AR_X),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
                                 SRC0_ABS(0),
                                 SRC1_ABS(0),
                                 UPDATE_EXECUTE_MASK(0),
                                 UPDATE_PRED(0),
                                 WRITE_MASK(1),
                                 FOG_MERGE(0),
                                 OMOD(SQ_ALU_OMOD_OFF),
                                 ALU_INST(SQ_OP2_INST_MUL),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(1),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_X),
                                 CLAMP(0));

    /* 23 srcY / h */
    shader[i++] = ALU_DWORD0(SRC0_SEL(1),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Y),
                             SRC0_NEG(0),
                             SRC1_SEL(257),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_W),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_AR_X),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
                                 SRC0_ABS(0),
                                 SRC1_ABS(0),
                                 UPDATE_EXECUTE_MASK(0),
                                 UPDATE_PRED(0),
                                 WRITE_MASK(1),
                                 FOG_MERGE(0),
                                 OMOD(SQ_ALU_OMOD_OFF),
                                 ALU_INST(SQ_OP2_INST_MUL),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(1),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Y),
                                 CLAMP(0));

    /* 24 maskX / w */
    shader[i++] = ALU_DWORD0(SRC0_SEL(0),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(258),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_W),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_AR_X),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
                                 SRC0_ABS(0),
                                 SRC1_ABS(0),
                                 UPDATE_EXECUTE_MASK(0),
                                 UPDATE_PRED(0),
                                 WRITE_MASK(1),
                                 FOG_MERGE(0),
                                 OMOD(SQ_ALU_OMOD_OFF),
                                 ALU_INST(SQ_OP2_INST_MUL),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_X),
                                 CLAMP(0));

    /* 25 maskY / h */
    shader[i++] = ALU_DWORD0(SRC0_SEL(0),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Y),
                             SRC0_NEG(0),
                             SRC1_SEL(259),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_W),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_AR_X),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
                                 SRC0_ABS(0),
                                 SRC1_ABS(0),
                                 UPDATE_EXECUTE_MASK(0),
                                 UPDATE_PRED(0),
                                 WRITE_MASK(1),
                                 FOG_MERGE(0),
                                 OMOD(SQ_ALU_OMOD_OFF),
                                 ALU_INST(SQ_OP2_INST_MUL),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Y),
                                 CLAMP(0));

    /* 26 srcX MAD - non-mask */
    shader[i++] = ALU_DWORD0(SRC0_SEL(256),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Y),
                             SRC0_NEG(0),
                             SRC1_SEL(0),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Y),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(256),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Z),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Z),
                                 CLAMP(0));
    /* 27 srcY MAD */
    shader[i++] = ALU_DWORD0(SRC0_SEL(257),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Y),
                             SRC0_NEG(0),
                             SRC1_SEL(0),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_Y),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(257),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Z),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_W),
                                 CLAMP(0));

    /* 28 srcX MAD */
    shader[i++] = ALU_DWORD0(SRC0_SEL(256),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(0),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(0));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(0),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_Z),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_X),
                                 CLAMP(0));
    /* 29 srcY MAD */
    shader[i++] = ALU_DWORD0(SRC0_SEL(257),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(0),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_X),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_LOOP),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP3(SRC2_SEL(0),
                                 SRC2_REL(ABSOLUTE),
                                 SRC2_ELEM(ELEM_W),
                                 SRC2_NEG(0),
                                 ALU_INST(SQ_OP3_INST_MULADD),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Y),
                                 CLAMP(0));
    /* 30 srcX / w */
    shader[i++] = ALU_DWORD0(SRC0_SEL(0),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_X),
                             SRC0_NEG(0),
                             SRC1_SEL(256),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_W),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_AR_X),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
                                 SRC0_ABS(0),
                                 SRC1_ABS(0),
                                 UPDATE_EXECUTE_MASK(0),
                                 UPDATE_PRED(0),
                                 WRITE_MASK(1),
                                 FOG_MERGE(0),
                                 OMOD(SQ_ALU_OMOD_OFF),
                                 ALU_INST(SQ_OP2_INST_MUL),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_X),
                                 CLAMP(0));

    /* 31 srcY / h */
    shader[i++] = ALU_DWORD0(SRC0_SEL(0),
                             SRC0_REL(ABSOLUTE),
                             SRC0_ELEM(ELEM_Y),
                             SRC0_NEG(0),
                             SRC1_SEL(257),
                             SRC1_REL(ABSOLUTE),
                             SRC1_ELEM(ELEM_W),
                             SRC1_NEG(0),
                             INDEX_MODE(SQ_INDEX_AR_X),
                             PRED_SEL(SQ_PRED_SEL_OFF),
                             LAST(1));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
                                 SRC0_ABS(0),
                                 SRC1_ABS(0),
                                 UPDATE_EXECUTE_MASK(0),
                                 UPDATE_PRED(0),
                                 WRITE_MASK(1),
                                 FOG_MERGE(0),
                                 OMOD(SQ_ALU_OMOD_OFF),
                                 ALU_INST(SQ_OP2_INST_MUL),
                                 BANK_SWIZZLE(SQ_ALU_VEC_012),
                                 DST_GPR(0),
                                 DST_REL(ABSOLUTE),
                                 DST_ELEM(ELEM_Y),
                                 CLAMP(0));

    /* 32/33 - dst - mask */
    shader[i++] = VTX_DWORD0(VTX_INST(SQ_VTX_INST_FETCH),
			     FETCH_TYPE(SQ_VTX_FETCH_VERTEX_DATA),
			     FETCH_WHOLE_QUAD(0),
			     BUFFER_ID(0),
			     SRC_GPR(0),
			     SRC_REL(ABSOLUTE),
			     SRC_SEL_X(SQ_SEL_X),
			     MEGA_FETCH_COUNT(24));
    shader[i++] = VTX_DWORD1_GPR(DST_GPR(2),
				 DST_REL(0),
				 DST_SEL_X(SQ_SEL_X),
				 DST_SEL_Y(SQ_SEL_Y),
				 DST_SEL_Z(SQ_SEL_0),
				 DST_SEL_W(SQ_SEL_1),
				 USE_CONST_FIELDS(0),
				 DATA_FORMAT(FMT_32_32_FLOAT),
				 NUM_FORMAT_ALL(SQ_NUM_FORMAT_SCALED),
				 FORMAT_COMP_ALL(SQ_FORMAT_COMP_SIGNED),
				 SRF_MODE_ALL(SRF_MODE_ZERO_CLAMP_MINUS_ONE));
    shader[i++] = VTX_DWORD2(OFFSET(0),
			     ENDIAN_SWAP(ENDIAN_NONE),
			     CONST_BUF_NO_STRIDE(0),
			     MEGA_FETCH(1));
    shader[i++] = VTX_DWORD_PAD;
    /* 34/35 - src */
    shader[i++] = VTX_DWORD0(VTX_INST(SQ_VTX_INST_FETCH),
			     FETCH_TYPE(SQ_VTX_FETCH_VERTEX_DATA),
			     FETCH_WHOLE_QUAD(0),
			     BUFFER_ID(0),
			     SRC_GPR(0),
			     SRC_REL(ABSOLUTE),
			     SRC_SEL_X(SQ_SEL_X),
			     MEGA_FETCH_COUNT(8));
    shader[i++] = VTX_DWORD1_GPR(DST_GPR(1),
				 DST_REL(0),
				 DST_SEL_X(SQ_SEL_X),
				 DST_SEL_Y(SQ_SEL_Y),
				 DST_SEL_Z(SQ_SEL_1),
				 DST_SEL_W(SQ_SEL_0),
				 USE_CONST_FIELDS(0),
				 DATA_FORMAT(FMT_32_32_FLOAT),
				 NUM_FORMAT_ALL(SQ_NUM_FORMAT_SCALED),
				 FORMAT_COMP_ALL(SQ_FORMAT_COMP_SIGNED),
				 SRF_MODE_ALL(SRF_MODE_ZERO_CLAMP_MINUS_ONE));
    shader[i++] = VTX_DWORD2(OFFSET(8),
			     ENDIAN_SWAP(ENDIAN_NONE),
			     CONST_BUF_NO_STRIDE(0),
			     MEGA_FETCH(0));
    shader[i++] = VTX_DWORD_PAD;
    /* 36/37 - mask */
    shader[i++] = VTX_DWORD0(VTX_INST(SQ_VTX_INST_FETCH),
			     FETCH_TYPE(SQ_VTX_FETCH_VERTEX_DATA),
			     FETCH_WHOLE_QUAD(0),
			     BUFFER_ID(0),
			     SRC_GPR(0),
			     SRC_REL(ABSOLUTE),
			     SRC_SEL_X(SQ_SEL_X),
			     MEGA_FETCH_COUNT(8));
    shader[i++] = VTX_DWORD1_GPR(DST_GPR(0),
				 DST_REL(0),
				 DST_SEL_X(SQ_SEL_X),
				 DST_SEL_Y(SQ_SEL_Y),
				 DST_SEL_Z(SQ_SEL_1),
				 DST_SEL_W(SQ_SEL_0),
				 USE_CONST_FIELDS(0),
				 DATA_FORMAT(FMT_32_32_FLOAT),
				 NUM_FORMAT_ALL(SQ_NUM_FORMAT_SCALED),
				 FORMAT_COMP_ALL(SQ_FORMAT_COMP_SIGNED),
				 SRF_MODE_ALL(SRF_MODE_ZERO_CLAMP_MINUS_ONE));
    shader[i++] = VTX_DWORD2(OFFSET(16),
			     ENDIAN_SWAP(ENDIAN_NONE),
			     CONST_BUF_NO_STRIDE(0),
			     MEGA_FETCH(0));
    shader[i++] = VTX_DWORD_PAD;

    /* 38/39 - dst - non-mask */
    shader[i++] = VTX_DWORD0(VTX_INST(SQ_VTX_INST_FETCH),
			     FETCH_TYPE(SQ_VTX_FETCH_VERTEX_DATA),
			     FETCH_WHOLE_QUAD(0),
			     BUFFER_ID(0),
			     SRC_GPR(0),
			     SRC_REL(ABSOLUTE),
			     SRC_SEL_X(SQ_SEL_X),
			     MEGA_FETCH_COUNT(16));
    shader[i++] = VTX_DWORD1_GPR(DST_GPR(1),
				 DST_REL(0),
				 DST_SEL_X(SQ_SEL_X),
				 DST_SEL_Y(SQ_SEL_Y),
				 DST_SEL_Z(SQ_SEL_0),
				 DST_SEL_W(SQ_SEL_1),
				 USE_CONST_FIELDS(0),
				 DATA_FORMAT(FMT_32_32_FLOAT),
				 NUM_FORMAT_ALL(SQ_NUM_FORMAT_SCALED),
				 FORMAT_COMP_ALL(SQ_FORMAT_COMP_SIGNED),
				 SRF_MODE_ALL(SRF_MODE_ZERO_CLAMP_MINUS_ONE));
    shader[i++] = VTX_DWORD2(OFFSET(0),
			     ENDIAN_SWAP(ENDIAN_NONE),
			     CONST_BUF_NO_STRIDE(0),
			     MEGA_FETCH(1));
    shader[i++] = VTX_DWORD_PAD;
    /* 40/41 - src */
    shader[i++] = VTX_DWORD0(VTX_INST(SQ_VTX_INST_FETCH),
			     FETCH_TYPE(SQ_VTX_FETCH_VERTEX_DATA),
			     FETCH_WHOLE_QUAD(0),
			     BUFFER_ID(0),
			     SRC_GPR(0),
			     SRC_REL(ABSOLUTE),
			     SRC_SEL_X(SQ_SEL_X),
			     MEGA_FETCH_COUNT(8));
    shader[i++] = VTX_DWORD1_GPR(DST_GPR(0),
				 DST_REL(0),
				 DST_SEL_X(SQ_SEL_X),
				 DST_SEL_Y(SQ_SEL_Y),
				 DST_SEL_Z(SQ_SEL_1),
				 DST_SEL_W(SQ_SEL_0),
				 USE_CONST_FIELDS(0),
				 DATA_FORMAT(FMT_32_32_FLOAT),
				 NUM_FORMAT_ALL(SQ_NUM_FORMAT_SCALED),
				 FORMAT_COMP_ALL(SQ_FORMAT_COMP_SIGNED),
				 SRF_MODE_ALL(SRF_MODE_ZERO_CLAMP_MINUS_ONE));
    shader[i++] = VTX_DWORD2(OFFSET(8),
			     ENDIAN_SWAP(ENDIAN_NONE),
			     CONST_BUF_NO_STRIDE(0),
			     MEGA_FETCH(0));
    shader[i++] = VTX_DWORD_PAD;

    return i;
}

/* comp ps --------------------------------------- */
int R600_comp_ps(RADEONChipFamily ChipSet, uint32_t* shader)
{
    int i = 0;

    /* 0 */
    shader[i++] = CF_DWORD0(ADDR(3));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
                            CF_CONST(0),
                            COND(SQ_CF_COND_BOOL),
                            I_COUNT(0),
                            CALL_COUNT(0),
                            END_OF_PROGRAM(0),
                            VALID_PIXEL_MODE(0),
                            CF_INST(SQ_CF_INST_CALL),
                            WHOLE_QUAD_MODE(0),
                            BARRIER(0));
    /* 1 */
    shader[i++] = CF_DWORD0(ADDR(7));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
                            CF_CONST(0),
                            COND(SQ_CF_COND_NOT_BOOL),
                            I_COUNT(0),
                            CALL_COUNT(0),
                            END_OF_PROGRAM(0),
                            VALID_PIXEL_MODE(0),
                            CF_INST(SQ_CF_INST_CALL),
                            WHOLE_QUAD_MODE(0),
                            BARRIER(0));
    /* 2 */
    shader[i++] = CF_DWORD0(ADDR(0));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
                            CF_CONST(0),
                            COND(SQ_CF_COND_ACTIVE),
                            I_COUNT(0),
                            CALL_COUNT(0),
                            END_OF_PROGRAM(1),
                            VALID_PIXEL_MODE(0),
                            CF_INST(SQ_CF_INST_NOP),
                            WHOLE_QUAD_MODE(0),
                            BARRIER(1));

    /* 3 - mask sub */
    shader[i++] = CF_DWORD0(ADDR(14));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(2),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_TEX),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));

    /* 4 */
    shader[i++] = CF_ALU_DWORD0(ADDR(10),
				KCACHE_BANK0(0),
				KCACHE_BANK1(0),
				KCACHE_MODE0(SQ_CF_KCACHE_NOP));
    shader[i++] = CF_ALU_DWORD1(KCACHE_MODE1(SQ_CF_KCACHE_NOP),
				KCACHE_ADDR0(0),
				KCACHE_ADDR1(0),
				I_COUNT(4),
				USES_WATERFALL(0),
				CF_INST(SQ_CF_INST_ALU),
				WHOLE_QUAD_MODE(0),
				BARRIER(1));

    /* 5 */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(CF_PIXEL_MRT0),
					  TYPE(SQ_EXPORT_PIXEL),
					  RW_GPR(2),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(1));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_Z),
					       SRC_SEL_W(SQ_SEL_W),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(1),
					       END_OF_PROGRAM(0),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT_DONE),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(1));
    /* 6 */
    shader[i++] = CF_DWORD0(ADDR(0));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(0),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_RETURN),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));

    /* 7 non-mask sub */
    shader[i++] = CF_DWORD0(ADDR(18));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(1),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_TEX),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));
    /* 8 */
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(CF_PIXEL_MRT0),
					  TYPE(SQ_EXPORT_PIXEL),
					  RW_GPR(0),
					  RW_REL(ABSOLUTE),
					  INDEX_GPR(0),
					  ELEM_SIZE(1));
    shader[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					       SRC_SEL_Y(SQ_SEL_Y),
					       SRC_SEL_Z(SQ_SEL_Z),
					       SRC_SEL_W(SQ_SEL_W),
					       R6xx_ELEM_LOOP(0),
					       BURST_COUNT(1),
					       END_OF_PROGRAM(0),
					       VALID_PIXEL_MODE(0),
					       CF_INST(SQ_CF_INST_EXPORT_DONE),
					       WHOLE_QUAD_MODE(0),
					       BARRIER(1));
    /* 9 */
    shader[i++] = CF_DWORD0(ADDR(0));
    shader[i++] = CF_DWORD1(POP_COUNT(0),
			    CF_CONST(0),
			    COND(SQ_CF_COND_ACTIVE),
			    I_COUNT(0),
			    CALL_COUNT(0),
			    END_OF_PROGRAM(0),
			    VALID_PIXEL_MODE(0),
			    CF_INST(SQ_CF_INST_RETURN),
			    WHOLE_QUAD_MODE(0),
			    BARRIER(1));

    /* 10 - alu 0 */
    /* MUL gpr[2].x gpr[1].x gpr[0].x */
    shader[i++] = ALU_DWORD0(SRC0_SEL(1),
			     SRC0_REL(ABSOLUTE),
			     SRC0_ELEM(ELEM_X),
			     SRC0_NEG(0),
			     SRC1_SEL(0),
			     SRC1_REL(ABSOLUTE),
			     SRC1_ELEM(ELEM_X),
			     SRC1_NEG(0),
			     INDEX_MODE(SQ_INDEX_LOOP),
			     PRED_SEL(SQ_PRED_SEL_OFF),
			     LAST(0));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
				 SRC0_ABS(0),
				 SRC1_ABS(0),
				 UPDATE_EXECUTE_MASK(0),
				 UPDATE_PRED(0),
				 WRITE_MASK(1),
				 FOG_MERGE(0),
				 OMOD(SQ_ALU_OMOD_OFF),
				 ALU_INST(SQ_OP2_INST_MUL),
				 BANK_SWIZZLE(SQ_ALU_VEC_012),
				 DST_GPR(2),
				 DST_REL(ABSOLUTE),
				 DST_ELEM(ELEM_X),
				 CLAMP(1));
    /* 11 - alu 1 */
    /* MUL gpr[2].y gpr[1].y gpr[0].y */
    shader[i++] = ALU_DWORD0(SRC0_SEL(1),
			     SRC0_REL(ABSOLUTE),
			     SRC0_ELEM(ELEM_Y),
			     SRC0_NEG(0),
			     SRC1_SEL(0),
			     SRC1_REL(ABSOLUTE),
			     SRC1_ELEM(ELEM_Y),
			     SRC1_NEG(0),
			     INDEX_MODE(SQ_INDEX_LOOP),
			     PRED_SEL(SQ_PRED_SEL_OFF),
			     LAST(0));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
				 SRC0_ABS(0),
				 SRC1_ABS(0),
				 UPDATE_EXECUTE_MASK(0),
				 UPDATE_PRED(0),
				 WRITE_MASK(1),
				 FOG_MERGE(0),
				 OMOD(SQ_ALU_OMOD_OFF),
				 ALU_INST(SQ_OP2_INST_MUL),
				 BANK_SWIZZLE(SQ_ALU_VEC_012),
				 DST_GPR(2),
				 DST_REL(ABSOLUTE),
				 DST_ELEM(ELEM_Y),
				 CLAMP(1));
    /* 12 - alu 2 */
    /* MUL gpr[2].z gpr[1].z gpr[0].z */
    shader[i++] = ALU_DWORD0(SRC0_SEL(1),
			     SRC0_REL(ABSOLUTE),
			     SRC0_ELEM(ELEM_Z),
			     SRC0_NEG(0),
			     SRC1_SEL(0),
			     SRC1_REL(ABSOLUTE),
			     SRC1_ELEM(ELEM_Z),
			     SRC1_NEG(0),
			     INDEX_MODE(SQ_INDEX_LOOP),
			     PRED_SEL(SQ_PRED_SEL_OFF),
			     LAST(0));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
				 SRC0_ABS(0),
				 SRC1_ABS(0),
				 UPDATE_EXECUTE_MASK(0),
				 UPDATE_PRED(0),
				 WRITE_MASK(1),
				 FOG_MERGE(0),
				 OMOD(SQ_ALU_OMOD_OFF),
				 ALU_INST(SQ_OP2_INST_MUL),
				 BANK_SWIZZLE(SQ_ALU_VEC_012),
				 DST_GPR(2),
				 DST_REL(ABSOLUTE),
				 DST_ELEM(ELEM_Z),
				 CLAMP(1));
    /* 13 - alu 3 */
    /* MUL gpr[2].w gpr[1].w gpr[0].w */
    shader[i++] = ALU_DWORD0(SRC0_SEL(1),
			     SRC0_REL(ABSOLUTE),
			     SRC0_ELEM(ELEM_W),
			     SRC0_NEG(0),
			     SRC1_SEL(0),
			     SRC1_REL(ABSOLUTE),
			     SRC1_ELEM(ELEM_W),
			     SRC1_NEG(0),
			     INDEX_MODE(SQ_INDEX_LOOP),
			     PRED_SEL(SQ_PRED_SEL_OFF),
			     LAST(1));
    shader[i++] = ALU_DWORD1_OP2(ChipSet,
				 SRC0_ABS(0),
				 SRC1_ABS(0),
				 UPDATE_EXECUTE_MASK(0),
				 UPDATE_PRED(0),
				 WRITE_MASK(1),
				 FOG_MERGE(0),
				 OMOD(SQ_ALU_OMOD_OFF),
				 ALU_INST(SQ_OP2_INST_MUL),
				 BANK_SWIZZLE(SQ_ALU_VEC_012),
				 DST_GPR(2),
				 DST_REL(ABSOLUTE),
				 DST_ELEM(ELEM_W),
				 CLAMP(1));

    /* 14/15 - src - mask */
    shader[i++] = TEX_DWORD0(TEX_INST(SQ_TEX_INST_SAMPLE),
			     BC_FRAC_MODE(0),
			     FETCH_WHOLE_QUAD(0),
			     RESOURCE_ID(0),
			     SRC_GPR(0),
			     SRC_REL(ABSOLUTE),
			     R7xx_ALT_CONST(0));
    shader[i++] = TEX_DWORD1(DST_GPR(0),
			     DST_REL(ABSOLUTE),
			     DST_SEL_X(SQ_SEL_X),
			     DST_SEL_Y(SQ_SEL_Y),
			     DST_SEL_Z(SQ_SEL_Z),
			     DST_SEL_W(SQ_SEL_W),
			     LOD_BIAS(0),
			     COORD_TYPE_X(TEX_NORMALIZED),
			     COORD_TYPE_Y(TEX_NORMALIZED),
			     COORD_TYPE_Z(TEX_NORMALIZED),
			     COORD_TYPE_W(TEX_NORMALIZED));
    shader[i++] = TEX_DWORD2(OFFSET_X(0),
			     OFFSET_Y(0),
			     OFFSET_Z(0),
			     SAMPLER_ID(0),
			     SRC_SEL_X(SQ_SEL_X),
			     SRC_SEL_Y(SQ_SEL_Y),
			     SRC_SEL_Z(SQ_SEL_0),
			     SRC_SEL_W(SQ_SEL_1));
    shader[i++] = TEX_DWORD_PAD;
    /* 16/17 - mask */
    shader[i++] = TEX_DWORD0(TEX_INST(SQ_TEX_INST_SAMPLE),
			     BC_FRAC_MODE(0),
			     FETCH_WHOLE_QUAD(0),
			     RESOURCE_ID(1),
			     SRC_GPR(1),
			     SRC_REL(ABSOLUTE),
			     R7xx_ALT_CONST(0));
    shader[i++] = TEX_DWORD1(DST_GPR(1),
			     DST_REL(ABSOLUTE),
			     DST_SEL_X(SQ_SEL_X),
			     DST_SEL_Y(SQ_SEL_Y),
			     DST_SEL_Z(SQ_SEL_Z),
			     DST_SEL_W(SQ_SEL_W),
			     LOD_BIAS(0),
			     COORD_TYPE_X(TEX_NORMALIZED),
			     COORD_TYPE_Y(TEX_NORMALIZED),
			     COORD_TYPE_Z(TEX_NORMALIZED),
			     COORD_TYPE_W(TEX_NORMALIZED));
    shader[i++] = TEX_DWORD2(OFFSET_X(0),
			     OFFSET_Y(0),
			     OFFSET_Z(0),
			     SAMPLER_ID(1),
			     SRC_SEL_X(SQ_SEL_X),
			     SRC_SEL_Y(SQ_SEL_Y),
			     SRC_SEL_Z(SQ_SEL_0),
			     SRC_SEL_W(SQ_SEL_1));
    shader[i++] = TEX_DWORD_PAD;

    /* 18/19 - src - non-mask */
    shader[i++] = TEX_DWORD0(TEX_INST(SQ_TEX_INST_SAMPLE),
			     BC_FRAC_MODE(0),
			     FETCH_WHOLE_QUAD(0),
			     RESOURCE_ID(0),
			     SRC_GPR(0),
			     SRC_REL(ABSOLUTE),
			     R7xx_ALT_CONST(0));
    shader[i++] = TEX_DWORD1(DST_GPR(0),
			     DST_REL(ABSOLUTE),
			     DST_SEL_X(SQ_SEL_X),
			     DST_SEL_Y(SQ_SEL_Y),
			     DST_SEL_Z(SQ_SEL_Z),
			     DST_SEL_W(SQ_SEL_W),
			     LOD_BIAS(0),
			     COORD_TYPE_X(TEX_NORMALIZED),
			     COORD_TYPE_Y(TEX_NORMALIZED),
			     COORD_TYPE_Z(TEX_NORMALIZED),
			     COORD_TYPE_W(TEX_NORMALIZED));
    shader[i++] = TEX_DWORD2(OFFSET_X(0),
			     OFFSET_Y(0),
			     OFFSET_Z(0),
			     SAMPLER_ID(0),
			     SRC_SEL_X(SQ_SEL_X),
			     SRC_SEL_Y(SQ_SEL_Y),
			     SRC_SEL_Z(SQ_SEL_0),
			     SRC_SEL_W(SQ_SEL_1));
    shader[i++] = TEX_DWORD_PAD;

    return i;
}

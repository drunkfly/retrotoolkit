/*
 * (c) Copyright 2021 by Einar Saukas. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of its author may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define INITIAL_OFFSET 1

#define FALSE 0
#define TRUE 1

struct ZX0Context;
typedef struct ZX0Context ZX0Context;
ZX0Context* zx0_new(void);
void zx0_delete(ZX0Context* C);
#define BLOCK ZX0Block
#define block_t zx0block_t
typedef struct block_t {
    struct block_t *chain;
    struct block_t *ghost_chain;
    int bits;
    int index;
    int offset;
    int length;
    int references;
} BLOCK;

BLOCK *zx0_allocate(ZX0Context* C, int bits, int index, int offset, int length, BLOCK *chain);

void* zx0_assign(ZX0Context* C, BLOCK **ptr, BLOCK *chain);

BLOCK **zx0_optimize(ZX0Context* C, unsigned char *input_data, int input_size, int skip, int offset_limit);

unsigned char *zx0_compress(BLOCK *optimal, unsigned char *input_data, int input_size, int skip, int backwards_mode, int *output_size, int *delta);

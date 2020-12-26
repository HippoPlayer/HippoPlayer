#define IMPULSE_N_SHIFT 7
#define IMPULSE_W_SHIFT 5
static const short base_impulse[] = {
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,   256,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    -1,     1,    -2,   256,     2,    -1,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -4,   256,     4,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -6,   256,     6,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -2,     4,    -8,   256,     8,    -4,     2,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     5,    -9,   255,    10,    -5,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,    -1,     1,    -2,     2,    -3,     5,   -11,   256,    12,    -6,     4,    -2,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -4,     6,   -13,   255,    15,    -7,     4,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -4,     7,   -15,   255,    17,    -8,     5,    -3,     2,    -2,     1,    -1,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     1,    -1,     2,    -2,     3,    -5,     8,   -16,   254,    19,    -9,     5,    -4,     2,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     4,    -6,     9,   -18,   254,    21,   -10,     6,    -4,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     4,    -6,    10,   -20,   253,    23,   -11,     7,    -4,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     4,    -7,    11,   -21,   252,    26,   -12,     7,    -5,     3,    -2,     2,    -1,     1,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -2,     2,    -3,     5,    -7,    11,   -23,   251,    28,   -13,     8,    -5,     4,    -2,     2,    -1,     1,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -2,     2,    -4,     5,    -8,    12,   -25,   255,    31,   -14,     8,    -6,     4,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     5,    -8,    13,   -26,   252,    33,   -15,     9,    -6,     4,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -4,     6,    -9,    14,   -27,   249,    35,   -16,    10,    -6,     4,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -4,     6,    -9,    14,   -29,   249,    38,   -17,    10,    -7,     5,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -4,     6,    -9,    15,   -30,   248,    40,   -18,    11,    -7,     5,    -3,     2,    -2,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -5,     7,   -10,    16,   -32,   250,    43,   -19,    11,    -8,     5,    -4,     2,    -2,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -2,     3,    -5,     7,   -10,    17,   -33,   246,    45,   -20,    12,    -8,     5,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -2,     3,    -5,     7,   -11,    17,   -34,   245,    48,   -21,    12,    -8,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -2,     4,    -5,     8,   -11,    18,   -35,   242,    50,   -22,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -5,     8,   -12,    19,   -36,   242,    52,   -23,    14,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -6,     8,   -12,    19,   -37,   241,    55,   -24,    14,    -9,     6,    -4,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -6,     8,   -12,    20,   -38,   239,    58,   -25,    15,   -10,     7,    -5,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -6,     9,   -13,    20,   -39,   239,    60,   -26,    15,   -10,     7,    -5,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -6,     9,   -13,    21,   -40,   235,    62,   -26,    16,   -10,     7,    -5,     3,    -2,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -6,     9,   -14,    21,   -41,   235,    65,   -27,    16,   -11,     7,    -5,     4,    -2,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -6,     9,   -14,    22,   -42,   232,    67,   -28,    17,   -11,     8,    -5,     4,    -2,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     5,    -7,    10,   -14,    22,   -43,   231,    70,   -29,    17,   -11,     8,    -5,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     5,    -7,    10,   -15,    23,   -44,   231,    73,   -30,    18,   -12,     8,    -6,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     5,    -7,    10,   -15,    24,   -45,   229,    76,   -31,    19,   -12,     8,    -6,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     5,    -7,    10,   -15,    24,   -46,   228,    79,   -32,    19,   -13,     9,    -6,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     5,    -7,    11,   -16,    24,   -46,   226,    81,   -33,    20,   -13,     9,    -6,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     2,    -4,     5,    -7,    11,   -16,    25,   -47,   226,    84,   -34,    20,   -13,     9,    -6,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     2,    -4,     5,    -8,    11,   -16,    26,   -48,   225,    88,   -35,    21,   -14,     9,    -7,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     2,    -4,     5,    -8,    11,   -16,    26,   -49,   223,    90,   -36,    21,   -14,    10,    -7,     5,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     2,    -4,     5,    -8,    11,   -17,    26,   -50,   222,    93,   -37,    22,   -14,    10,    -7,     5,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     2,    -4,     6,    -8,    12,   -17,    27,   -50,   219,    95,   -38,    22,   -15,    10,    -7,     5,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -8,    12,   -17,    27,   -50,   216,    97,   -38,    22,   -15,    10,    -7,     5,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -8,    12,   -17,    27,   -50,   213,   100,   -39,    23,   -15,    10,    -7,     5,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -8,    12,   -18,    27,   -51,   212,   103,   -40,    23,   -15,    11,    -7,     5,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -8,    12,   -18,    28,   -51,   209,   105,   -40,    24,   -16,    11,    -7,     5,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    12,   -18,    28,   -52,   210,   110,   -42,    24,   -16,    11,    -8,     5,    -4,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    12,   -18,    29,   -53,   209,   113,   -43,    25,   -16,    11,    -8,     5,    -4,     2,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    29,   -53,   208,   116,   -44,    25,   -17,    11,    -8,     5,    -4,     2,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    29,   -53,   205,   118,   -44,    26,   -17,    11,    -8,     5,    -4,     2,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    29,   -54,   202,   121,   -45,    26,   -17,    12,    -8,     6,    -4,     2,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    29,   -54,   199,   123,   -45,    26,   -17,    12,    -8,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    29,   -54,   197,   126,   -46,    26,   -17,    12,    -8,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    29,   -54,   194,   128,   -46,    27,   -17,    12,    -8,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -54,   192,   131,   -47,    27,   -18,    12,    -8,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -54,   190,   134,   -48,    27,   -18,    12,    -8,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -54,   188,   136,   -48,    28,   -18,    12,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -54,   186,   139,   -49,    28,   -18,    12,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -54,   182,   142,   -49,    28,   -18,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -54,   181,   145,   -50,    28,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -54,   178,   147,   -50,    29,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -54,   176,   150,   -51,    29,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -54,   173,   153,   -51,    29,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -53,   170,   155,   -51,    29,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -53,   168,   158,   -52,    29,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -53,   165,   160,   -52,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -52,   162,   162,   -52,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    30,   -52,   160,   165,   -53,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    29,   -52,   158,   168,   -53,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    29,   -51,   155,   170,   -53,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    29,   -51,   153,   173,   -54,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    29,   -51,   150,   176,   -54,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    29,   -50,   147,   178,   -54,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -19,    28,   -50,   145,   181,   -54,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -18,    28,   -49,   142,   182,   -54,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    12,   -18,    28,   -49,   139,   186,   -54,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    12,   -18,    28,   -48,   136,   188,   -54,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -8,    12,   -18,    27,   -48,   134,   190,   -54,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -8,    12,   -18,    27,   -47,   131,   192,   -54,    30,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -8,    12,   -17,    27,   -46,   128,   194,   -54,    29,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -8,    12,   -17,    26,   -46,   126,   197,   -54,    29,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -8,    12,   -17,    26,   -45,   123,   199,   -54,    29,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     2,    -4,     6,    -8,    12,   -17,    26,   -45,   121,   202,   -54,    29,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     2,    -4,     5,    -8,    11,   -17,    26,   -44,   118,   205,   -53,    29,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     2,    -4,     5,    -8,    11,   -17,    25,   -44,   116,   208,   -53,    29,   -19,    13,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,    -1,     1,    -2,     2,    -4,     5,    -8,    11,   -16,    25,   -43,   113,   209,   -53,    29,   -18,    12,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -4,     5,    -8,    11,   -16,    24,   -42,   110,   210,   -52,    28,   -18,    12,    -9,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     5,    -7,    11,   -16,    24,   -40,   105,   209,   -51,    28,   -18,    12,    -8,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     5,    -7,    11,   -15,    23,   -40,   103,   212,   -51,    27,   -18,    12,    -8,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     5,    -7,    10,   -15,    23,   -39,   100,   213,   -50,    27,   -17,    12,    -8,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     5,    -7,    10,   -15,    22,   -38,    97,   216,   -50,    27,   -17,    12,    -8,     6,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     5,    -7,    10,   -15,    22,   -38,    95,   219,   -50,    27,   -17,    12,    -8,     6,    -4,     2,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     5,    -7,    10,   -14,    22,   -37,    93,   222,   -50,    26,   -17,    11,    -8,     5,    -4,     2,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     5,    -7,    10,   -14,    21,   -36,    90,   223,   -49,    26,   -16,    11,    -8,     5,    -4,     2,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -7,     9,   -14,    21,   -35,    88,   225,   -48,    26,   -16,    11,    -8,     5,    -4,     2,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -6,     9,   -13,    20,   -34,    84,   226,   -47,    25,   -16,    11,    -7,     5,    -4,     2,    -2,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -6,     9,   -13,    20,   -33,    81,   226,   -46,    24,   -16,    11,    -7,     5,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -6,     9,   -13,    19,   -32,    79,   228,   -46,    24,   -15,    10,    -7,     5,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -6,     8,   -12,    19,   -31,    76,   229,   -45,    24,   -15,    10,    -7,     5,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -6,     8,   -12,    18,   -30,    73,   231,   -44,    23,   -15,    10,    -7,     5,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -3,     4,    -5,     8,   -11,    17,   -29,    70,   231,   -43,    22,   -14,    10,    -7,     5,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -2,     4,    -5,     8,   -11,    17,   -28,    67,   232,   -42,    22,   -14,     9,    -6,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -2,     4,    -5,     7,   -11,    16,   -27,    65,   235,   -41,    21,   -14,     9,    -6,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     2,    -2,     3,    -5,     7,   -10,    16,   -26,    62,   235,   -40,    21,   -13,     9,    -6,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -5,     7,   -10,    15,   -26,    60,   239,   -39,    20,   -13,     9,    -6,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -5,     7,   -10,    15,   -25,    58,   239,   -38,    20,   -12,     8,    -6,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -4,     6,    -9,    14,   -24,    55,   241,   -37,    19,   -12,     8,    -6,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    14,   -23,    52,   242,   -36,    19,   -12,     8,    -5,     4,    -3,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -9,    13,   -22,    50,   242,   -35,    18,   -11,     8,    -5,     4,    -2,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     6,    -8,    12,   -21,    48,   245,   -34,    17,   -11,     7,    -5,     3,    -2,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -2,     3,    -4,     5,    -8,    12,   -20,    45,   246,   -33,    17,   -10,     7,    -5,     3,    -2,     2,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -2,     2,    -4,     5,    -8,    11,   -19,    43,   250,   -32,    16,   -10,     7,    -5,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -2,     2,    -3,     5,    -7,    11,   -18,    40,   248,   -30,    15,    -9,     6,    -4,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     5,    -7,    10,   -17,    38,   249,   -29,    14,    -9,     6,    -4,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     4,    -6,    10,   -16,    35,   249,   -27,    14,    -9,     6,    -4,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     4,    -6,     9,   -15,    33,   252,   -26,    13,    -8,     5,    -4,     3,    -2,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     4,    -6,     8,   -14,    31,   255,   -25,    12,    -8,     5,    -4,     2,    -2,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     1,    -1,     2,    -2,     4,    -5,     8,   -13,    28,   251,   -23,    11,    -7,     5,    -3,     2,    -2,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     1,    -1,     2,    -2,     3,    -5,     7,   -12,    26,   252,   -21,    11,    -7,     4,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -4,     7,   -11,    23,   253,   -20,    10,    -6,     4,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -4,     6,   -10,    21,   254,   -18,     9,    -6,     4,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     2,    -4,     5,    -9,    19,   254,   -16,     8,    -5,     3,    -2,     2,    -1,     1,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,    -1,     1,    -2,     2,    -3,     5,    -8,    17,   255,   -15,     7,    -4,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     4,    -7,    15,   255,   -13,     6,    -4,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -2,     4,    -6,    12,   256,   -11,     5,    -3,     2,    -2,     1,    -1,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     3,    -5,    10,   255,    -9,     5,    -3,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     2,    -4,     8,   256,    -8,     4,    -2,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    -1,     1,    -1,     2,    -3,     6,   256,    -6,     3,    -2,     1,    -1,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     1,    -1,     1,    -2,     4,   256,    -4,     2,    -1,     1,    -1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     1,    -1,     2,   256,    -2,     1,    -1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
};

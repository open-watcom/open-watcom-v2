struct V {
};

struct S : V {
};

struct T : S, virtual V {
};

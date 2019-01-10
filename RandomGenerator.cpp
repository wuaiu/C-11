struct RandomGenerator {

    std::mt19937 mt;

    /// random positive integer
    int rand_int ();

    /// random long
    long rand_long ();

    /// generate random integer between 0 and max-1
    int rand_int (int max);

    /// between 0 and 1
    float rand_float ();

    double rand_double ();

    explicit RandomGenerator (long seed = 1234);
};

RandomGenerator::RandomGenerator (long seed)
    : mt((unsigned int)seed) {}

int RandomGenerator::rand_int ()
{
    return mt() & 0x7fffffff;
}

long RandomGenerator::rand_long ()
{
    return long(rand_int()) | long(rand_int()) << 31;
}

int RandomGenerator::rand_int (int max)
{
    return mt() % max;
}

float RandomGenerator::rand_float ()
{
    return mt() / float(mt.max());
}

double RandomGenerator::rand_double ()
{
    return mt() / double(mt.max());
}

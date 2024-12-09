#include "test.h"

//RJG - this is revosim test system. Called from main window, friend of simulation manager, so can access private functions.
//All tests follow similar structure - you can write outputs to out stream, and return testFlag as true if passed, false fi failed.
//All pretty obvious, but shout if you have queries.

test::test()
{

}


bool test::callTest(int i, QString &outString)
{
    bool pass;
    switch (i)
    {
    case 0:
        pass = testZero(outString);
        return pass;
    case 1:
        pass = testOne(outString);
        return pass;
    case 2:
        pass = testTwo(outString);
        return pass;
    case 3:
        pass = testThree(outString);
        return pass;
    case 4:
        pass = testFour(outString);
        return pass;
    case 5:
        pass = testFive(outString);
        return pass;
    case 6:
        pass = testSix(outString);
        return pass;
    case 7:
        pass = testSeven(outString);
        return pass;
    case 8:
        pass = testEight(outString);
        return pass;
    case 9:
        pass = testNine(outString);
        return pass;
    case 10:
        pass = testTen(outString);
        return pass;
    case 11:
        pass = testEleven(outString);
        return pass;
    case 12:
        pass = testTwelve(outString);
        return pass;
    case 13:
        pass = testThirteen(outString);
        return pass;
    case 14:
        pass = testFourteen(outString);
        return pass;
    case 15:
        pass = testFifteen(outString);
        return pass;
    case 16:
        pass = testSixteen(outString);
        return pass;
    case 17:
        pass = testSeventeen(outString);
        return pass;
    case 18:
        pass = testEighteen(outString);
        return pass;
    case 19:
        pass = testNineteen(outString);
        return pass;
    case 20:
        pass = testTwenty(outString);
        return pass;
    }
    return false;
}

//RJG - testing random number generator (implemented when revosim switches to QRandomNumberGenerator
bool test::testZero(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Running test on random number functions (these run off QT's QRandomGenerator if you want to read about the sytsem).";

    int test8[256] = {0};
    for (int i = 0; i < 256000; i++)test8[simulationManager->simulationRandoms->rand8()]++;

    int min = 10000;
    int max = 0;

    for (auto r : test8)
    {
        if (r < min)min = r;
        if (r > max)max = r;
    }

    out << " Created 256000 random 8 bit numbers. Should have gotten each ~1000 times. Minimum was " << min << " maximum was " << max << ".";
    if (min < 850 || max > 1150)
    {
        testFlag = false;
        out << "**Fail here**";
    }

    QVector <int> test8seq(25, 0);
    for (auto &r : test8seq)r = simulationManager->simulationRandoms->rand8();
    QVector <int> test8seq2(25, 0);
    for (int i = 0; i < 256000; i++)
    {
        test8seq2.removeFirst();
        test8seq2.append(simulationManager->simulationRandoms->rand8());

        if (test8seq == test8seq2)
        {
            testFlag = false;

            out << "\n\nTest failed. Have identified 25 repeated characters, test sequence:\n";
            for (auto &r : test8seq) out << r;
            out << "\nRandoms sequence:\n";
            for (auto &r : test8seq2) out << r;
            out << "\n\n";
        }
    }

    int test16[65536] = {0};
    for (int i = 0; i < 65536000; i++)test16[simulationManager->simulationRandoms->rand16()]++;

    min = 10000;
    max = 0;

    for (auto r : test16)
    {
        if (r < min)min = r;
        if (r > max)max = r;
    }

    out << "\n\nCreated 65536000 random 16 bit numbers. Should have gotten each ~1000 times. Minimum was " << min << " maximum was " << max << ".";
    if (min < 800 || max > 1200)
    {
        testFlag = false;
        out << "**Fail here**";
    }
    quint64 min32 = 1000;
    quint64 max32 = 0;

    for (int i = 0; i < 10000000; i++)
    {
        quint64 random = simulationManager->simulationRandoms->rand32();
        if (random < min32) min32 = random;
        if (random > max32) max32 = random;
    }

    out << "\n\nNow created 100000000 random 32 bit numbers - checking instead for max and min to make sure we're using the full range, Minimum was " << min32 <<
        " highest distance away from maximum was  " << 4294967195 - max32 << ".";
    if (min32 > 1000 || max32 < 4294966295)
    {
        testFlag = false;
        out << "**Fail here**";
    }

    quint32 maxPoss32 = ~0;
    quint64 maxPoss64 = ~0;
    quint64 min64 = maxPoss64;
    quint64 max64 = 0;

    for (quint32 i = 0; i < 10000000; i++)
    {
        quint64 random = simulationManager->simulationRandoms->rand64();
        if (random < min64) min64 = random;
        if (random > max64) max64 = random;
    }

    out << "\n\nNow created 10000000 random 64 bit numbers - checking that the largest is greater than 2<sup>32</sup> (it is" << max64 << "). This ensures the random is at least >32 bit.";
    if (max64 < (maxPoss32 + 1000))
    {
        testFlag = false;
        out << "**Fail here**";
    }

    if (testFlag) out << "\n\nRandoms test passed.\n";
    //QString flagString = testFlag ? "true" : "false";
    if (!testFlag) out << "\n\nRandoms test failed but this does happen occasionally given the nature of random numbers. Perhaps run again. If it is still failing feel free to contact RJG.\n";

    return testFlag;
}

//RJG - REvoSim has a bunch of lookups that are integral to the whole thing - test these here
bool test::testOne(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing lookup tables and other simulation structures.";

    out << "First tweakers - should have a single 1 in each(64 bit left, 32 bit right):\n";

    for (int j = 0; j < 64 ; j++)
    {
        std::bitset<64> tweaker64BitSet(tweakers64[j]);
        for (int i = 0; i < 64 ; i++) tweaker64BitSet[i] ? out << 1 : out << 0;


        if (j < 32)
        {
            out << "  |  ";
            std::bitset<32> tweakerBitSet(tweakers[j]);
            for (int i = 0; i < 32 ; i++) tweakerBitSet[i] ? out << 1 : out << 0;
        }
        out << "\n";
    }
    out << "\n";

    for (quint32 n = 0; n < 65536; n++)
    {
        std::bitset<32> bitCount(n);
        quint32 count = 0;
        for (int i = 0; i < 32; i++)if (bitCount[i])count++;
        if (count != bitCounts[n])
        {
            out << "Error in bit counts for " << n << "\n";
            testFlag = false;
        }
    }
    if (testFlag) out << "Have also tested bit counts and all are in order\n\n";


    for (int i = 0; i < 255; i++)
    {
        quint32 R = (simulationManager->environmentalFitnessSytem->returnXOR(i, 0) ^ simulationManager->environmentalFitnessSytem->returnXOR(i + 1, 0));
        std::bitset<32> tweakerBitSetR(R);
        quint32 Rcount = 0;
        for (int i = 0; i < 32 ; i++) if (tweakerBitSetR[i]) Rcount++;
        if (Rcount != 1)
        {
            testFlag = false;
            out << "Rcount error - i " << i << " RCount " << Rcount << "\n";
        }

        quint32 G = (simulationManager->environmentalFitnessSytem->returnXOR(i, 1) ^ simulationManager->environmentalFitnessSytem->returnXOR(i + 1, 1));
        std::bitset<32> tweakerBitSetG(G);
        quint32 Gcount = 0;
        for (int i = 0; i < 32 ; i++) if (tweakerBitSetG[i]) Gcount++;
        if (Gcount != 1)
        {
            testFlag = false;
            out << "Gcount error - i " << i << " GCount " << Gcount << "\n";
        }

        quint32 B = (simulationManager->environmentalFitnessSytem->returnXOR(i, 2) ^ simulationManager->environmentalFitnessSytem->returnXOR(i + 1, 2));
        std::bitset<32> tweakerBitSetB(B);
        quint32 Bcount = 0;
        for (int i = 0; i < 32 ; i++) if (tweakerBitSetB[i]) Bcount++;
        if (Bcount != 1)
        {
            testFlag = false;
            out << "Bcount error - i " << i << " BCount " << Bcount << "\n";
        }
    }

    if (testFlag) out << "Tested to check that masks are 1 bit apart from 0 to 255 - no errors.\n\n";

    quint64 genexCount = 0;
    for (auto n : genex)
    {
        std::bitset<32> genexBitSet(n);
        for (int i = 0; i < 32 ; i++) if (genexBitSet[i]) genexCount++;
    }

    double mean (static_cast<double>(genexCount) / 65336.);
    out << "Checked 32 bit genex random bit combs, 16 bits should be on. Mean is " << mean << ".\n\n";
    if (mean < 15.5 || mean > 16.5)
    {
        testFlag = false;
        out << "Consider this out of range - returned error";
    }

    if (testFlag) out << "Lookup tests passed\n\n";

    return testFlag;
}

//RJG - multiword genomes allows us to break out important functions into their own classes. Test fitness here.
bool test::testTwo(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing fitness algorithm - creating fitness system, sending it different word lengths with known genome and environmental masks.\n";

    EnvironmentFitnessSystem *environmentalFitnessSytem = new EnvironmentFitnessSystem();
    environmentalFitnessSytem->setGenomeWordsFromString("0", 2);

    //RJG - set masks to zero, genome words to 1 - outputs should always me max possible value
    for (int i = 0; i < 256; i++)
        for (int j = 0; j < 3; j++)
            environmentalFitnessSytem->setXOR(i, j, 0);
    quint32 *genomeWords = new quint32[MAX_GENOME_WORDS];
    for (int i = 0; i < MAX_GENOME_WORDS; i++)genomeWords[i] = ~0;
    quint8 *environment = simulationManager->env->returnRGB(50, 50);

    //RJG - Test 1 word
    int count = environmentalFitnessSytem->calculateFitness(genomeWords, environment);
    int target = environmentalFitnessSytem->returnUseGenomeWordsCount() * 3 * 32;
    out << "Fitness should be " << target << " is " << count << ".\n";
    if (count != target)
    {
        testFlag = false;
        out << "Test failed.\n";
    }

    //RJG - Test 2 words
    environmentalFitnessSytem->setGenomeWordsFromString("01", 2);
    count = environmentalFitnessSytem->calculateFitness(genomeWords, environment);
    target = environmentalFitnessSytem->returnUseGenomeWordsCount() * 3 * 32;
    out << "Fitness should be " << target << " is " << count << ".\n";
    if (count != target)
    {
        testFlag = false;
        out << "Test failed.\n";
    }

    //RJG - Test 4 words
    environmentalFitnessSytem->setGenomeWordsFromString("014A", 32);
    count = environmentalFitnessSytem->calculateFitness(genomeWords, environment);
    target = environmentalFitnessSytem->returnUseGenomeWordsCount() * 3 * 32;
    out << "Fitness should be " << target << " is " << count << ".\n";
    if (count != target)
    {
        testFlag = false;
        out << "Test failed.\n";
    }

    //RJG - redo lookups to fix masks
    simulationManager->makeLookups();

    if (testFlag) out << "Tests passed.\n\n";
    return testFlag;
}

//RJG - test base system functions
bool test::testThree(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    System *system = new System("testSystem");

        for (int i = 0; i < 100; i++)
        {
        quint32 random = QRandomGenerator::global()->generate();
        std::bitset<32> tweakerBitSetB(random);
        int count1 = 0;
        for (int i = 0; i < 32 ; i++) if (tweakerBitSetB[i]) count1++;
        int count2 =  system->bitCount(random);
        if (count1 != count2) testFlag = false;
    }
    if (testFlag == false) out << "Failed at bitcounts.\n";
    else out << "Tested bitcounts for 100 random numbers - all were correct.\n";

    system->setGenomeWordsFromString("01", MAX_GENOME_WORDS);
    if (system->returnUseGenomeWordsCount() != 2) testFlag = false;
    if (testFlag == false) out << "Failed at setting words 1.\n";
    if (system->returnGenomeWordInUse(0) == false || system->returnGenomeWordInUse(1) == false) testFlag = false;
    if (testFlag == false) out << "Failed at setting words 2.\n";
    for (int i = 2; i < MAX_GENOME_WORDS; i++) if (!system->returnGenomeWordInUse(i) == false) testFlag = false;
    if (testFlag == false) out << "Failed at setting words 3.\n";
    qInfo() << "Running a test - system will provide a fail message to command line as result.";
    system->setGenomeWordsFromString("018B", MAX_GENOME_WORDS);
    if (system->returnUseGenomeWordsCount() != 4) testFlag = false;
    if (testFlag == false) out << "Failed at setting words 4.\n";
    if (system->setGenomeWordsFromString("018B", 2))testFlag = false;
    if (testFlag == false) out << "Failed at setting words 5.\n";
    if (testFlag) out << "Tested setting words within system - all tests passed.\n";

    int count = system->bitCount(0);
    if (count != 0)
    {
        testFlag = false;
        out << "Bit count failed 1.\n";
    }
    count = system->bitCount(~0);
    if (count != 32)
    {
        testFlag = false;
        out << "Bit count failed 2.\n";
    }
    std::bitset<32> testBitCount(0);
    for (int i = 0; i < 32; i++)if (i % 2 == 0)testBitCount[i] = 1;
    quint64 testBitCount64 = testBitCount.to_ulong();
    quint32 testBitCount32 = static_cast<quint32>(testBitCount64);
    count = system->bitCount(testBitCount32);
    if (count != 16)
    {
        testFlag = false;
        out << "Bit count failed 3.\n";
    }
    if (testFlag) out << "Tested system bitcounts - all tests passed.\n";

    if (testFlag) out << "Tests passed.\n\n";
    return testFlag;
}

//RJG - test printing and string functions
bool test::testFour(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing print genome functions in simulation manager.\n\n";

    QString genome(simulationManager->printGenome(static_cast<quint32>(~0)));
    QRegularExpression regEx("^[1]*$", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = regEx.match(genome);
    if (!match.hasMatch()) testFlag = false;
    out << "Printing 32 bit number " <<  QString::number(static_cast<quint32>(~0), 10) << " which should be all ones. Printed " << genome << ".\n";

    genome = (simulationManager->printGenome(static_cast<quint64>(~0)));
    match = regEx.match(genome);
    if (!match.hasMatch()) testFlag = false;
    out << "Printing 64 bit number " <<  QString::number(static_cast<quint64>(~0), 10) << " which should be all ones. Printed " << genome << ".\n";

    genome = (simulationManager->printGenome(static_cast<quint32>(0)));
    regEx.setPattern("^[0]*$");
    match = regEx.match(genome);
    if (!match.hasMatch()) testFlag = false;
    out << "Printing 32 bit number " <<  QString::number(static_cast<quint64>(0), 10) << " which should be all zeros. Printed " << genome << ".\n";

    genome = (simulationManager->printGenome(static_cast<quint64>(0)));
    match = regEx.match(genome);
    if (!match.hasMatch()) testFlag = false;
    out << "Printing 64 bit number " <<  QString::number(static_cast<quint64>(0), 10) << " which should be all zeros. Printed " << genome << ".\n";

    genome = (simulationManager->printGenome(static_cast<quint64>(0) | static_cast<quint32>(~0)));
    int count0 = 0, count1 = 0;
    for (int i = 0; i < genome.length(); i++)
        if (genome.at(i) == '0') count0++;
        else count1 ++;
    if (count0 != 32 || count1 != 32) testFlag = false;
    out << "Printing 64 bit number " <<  QString::number(static_cast<quint64>(0), 10) << " which should be all 32 ones and 32 zeros.";
    out << " Printed " << genome << " which has " << count0 << " zeros and " << count1 << " ones.\n";

    out << "\n\nTesting system print genome functions.\n\n ";

    System testSystem("Test system");
    testSystem.setGenomeWordsFromString("0", MAX_GENOME_WORDS);
    quint32 genomeWords[MAX_GENOME_WORDS];
    for (int i = 0; i < MAX_GENOME_WORDS; i++)genomeWords[i] = ~0;

    genome = testSystem.returnGenomeString(genomeWords, 10);
    regEx.setPattern("^[1]*$");
    match = regEx.match(genome);
    if (!match.hasMatch()) testFlag = false;
    out << "Printing first ten words of genome which should be all ones. Printed " << genome << ".\n";

    genome = (testSystem.returnGenomeInUseString(genomeWords));
    match = regEx.match(genome);
    if (!match.hasMatch()) testFlag = false;
    out << "Printing 32 bit number using words in use " <<  QString::number(static_cast<quint32>(~0), 10) << " which should be all ones. Printed " << genome << ".\n";


    if (testFlag) out << "\nTests passed.\n";
    else out << "\n\nTests failed.\n";

    return testFlag;
}

//Test reseed dialogue
bool test::testFive(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    Reseed reseedDialogue;
    quint32 max = static_cast<quint32>(~0);
    out << "Testing conversion of " << max << " to binary then hex strings, then back to numbers.\n";
    QString genome = simulationManager->printGenome(max);
    out << "As binary string: " << genome << ".\n";
    genome = reseedDialogue.convertStringToHex(genome, testFlag);
    out << "As hex string: " << genome << ".\n";
    quint32 numberHex = reseedDialogue.convertWordStringToNumber(genome, testFlag, 16);
    out << "Number from hex: " << numberHex << ".\n";
    if (numberHex != max) testFlag = false;
    genome = reseedDialogue.convertStringToBinary(genome, testFlag);
    out << "Converted back to binary string: " << genome << ".\n";
    quint32 number = reseedDialogue.convertWordStringToNumber(genome, testFlag, 2);
    out << "Number from binary: " << numberHex << ".\n";
    if (number != max) testFlag = false;

    quint32 min = static_cast<quint32>(0);
    out << "\nTesting conversion of " << min << " to binary then hex strings, then back to numbers.\n";
    genome = simulationManager->printGenome(min);
    out << "As binary string: " << genome << ".\n";
    genome = reseedDialogue.convertStringToHex(genome, testFlag);
    out << "As hex string: " << genome << ".\n";
    numberHex = reseedDialogue.convertWordStringToNumber(genome, testFlag, 16);
    out << "Number from hex: " << numberHex << ".\n";
    if (numberHex) testFlag = false;
    genome = reseedDialogue.convertStringToBinary(genome, testFlag);
    out << "Converted back to binary string: " << genome << ".\n";
    number = reseedDialogue.convertWordStringToNumber(genome, testFlag, 2);
    out << "Number from binary: " << numberHex << ".\n";
    if (number) testFlag = false;

    genome.replace(5, 1, '2');
    out << "\nNow checking validation on " << genome << " - should fail. ";
    if (reseedDialogue.validateString(genome, false)) testFlag = false;
    out << "Returned " << reseedDialogue.validateString(genome, false) << ".\nTesting this as a hex string - should return true. ";
    if (!reseedDialogue.validateString(genome, true))testFlag = false;
    out << "Returned " << reseedDialogue.validateString(genome, true) << ".\n";
    genome.replace(5, 1, 'G');
    out << "Now checking validation on " << genome << " as hex string - should fail. ";
    if (reseedDialogue.validateString(genome, true)) testFlag = false;
    out << "Returned " << reseedDialogue.validateString(genome, true) << ".\n";

    reseedDialogue.on_buttonBox_rejected();

    quint32 newGenome[MAX_GENOME_WORDS] = {0};
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++) newGenome[i] = simulationManager->simulationSettings->reseedGenome[i] = simulationManager->simulationRandoms->rand32();
    Reseed reseedDialogue2;
    reseedDialogue2.on_buttonBox_accepted();
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++)
        if (newGenome[i] != simulationManager->simulationSettings->reseedGenome[i])
        {
            out << "\nError detected writing new genome to simulation settings.\n";
            testFlag = false;
        }
    if (testFlag) out << "Tested newGenome write from reseed dialogue - no errors detected.\n";
    if (testFlag) out << "\nAll tests passed\n";
    return testFlag;
}

//Test interactions
bool test::testSix(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing interactions - first those that alter fitness. Start with blocks.";

    //EF - A priori ecological interactions table, from which more computationally efficient tables are constructed. Table arrangement is {row 1: 00_vs{00,01,10,11}, row 2: 01_vs{00,01,10,11}...}
    qint16 default_a_priori_interaction[4][4] = {{0, -2, -2, -4}, {2, 0, 0, -2}, {2, 0, 0, -2}, {4, 2, 2, 0}}; // Predator Grid
    qint16 backup_interaction[4][4];
    //RJG - this is a shortcut to assign the values without having to do for loops, whilst not assigning the values on initialisation in the header
    //First create backup of current interactions grid
    std::copy(&simulationManager->simulationSettings->a_priori_interaction[0][0], &simulationManager->simulationSettings->a_priori_interaction[0][0] + 16, &backup_interaction[0][0]);
    //Then set to default
    std::copy(&default_a_priori_interaction[0][0], &default_a_priori_interaction[0][0] + 16, &simulationManager->simulationSettings->a_priori_interaction[0][0]);
    simulationManager->makeInteractionLookups();

    out << "A priori interactions grid (which has been set to default values for this test):\n";
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++) out << simulationManager->simulationSettings->a_priori_interaction[i][j] << " ";
        out << "\n";
    }

    /*out << "\n\nFull interactions grid:\n";
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 256; j++) out << interaction[i][j] << "_";
        out << "\n";
    }*/

    quint32 testGenomeZero[2] = {0};
    quint32 max = ~0;
    quint32 testGenomeMax[2] = {max, max};
    quint32 testCombOne[2] = {2863311530, 2863311530}; //Threes in each two bits
    quint32 testCombTwo[2] = {1431655765, 1431655765}; //Ones in each two bits


    //Create interaction system here so it's easy to set the words etc.
    InteractionSystem testInteractionSystem;
    testInteractionSystem.setGenomeWordsFromString("01", MAX_GENOME_WORDS);

    int fitness = testInteractionSystem.performInteractFitness(testGenomeZero, testGenomeZero, 0, true);
    out << "Testing with all zeros - fitness change should be zero, is " << fitness << "\n";
    if (fitness != 0) testFlag = false;

    fitness = testInteractionSystem.performInteractFitness(testGenomeMax, testGenomeMax, 0, true);
    out << "Testing with all ones - fitness change should be zero, is " << fitness << "\n";
    if (fitness != 0) testFlag = false;

    fitness = testInteractionSystem.performInteractFitness(testGenomeZero, testGenomeMax, 0, true);
    out << "Testing with ones on zeros - this should hit -128, but due to lack of underflow on fitness this should come out as zero, is " << fitness << "\n";
    if (fitness != 0) testFlag = false;

    fitness = testInteractionSystem.performInteractFitness(testGenomeMax, testGenomeZero, 0, true);
    out << "Testing with zeros on ones - this should hit 128 (a value of four from the interaction table for each two bits in both genome words). Fitness change is " << fitness << "\n";
    if (fitness != 128) testFlag = false;

    fitness = testInteractionSystem.performInteractFitness(testCombOne, testGenomeZero, 0, true);
    out << "Testing with 2 (i.e. 10 in binary) on zeros - this should hit 64 (2 in the interactions table). Fitness change is " << fitness << "\n";
    if (fitness != 64) testFlag = false;

    fitness = testInteractionSystem.performInteractFitness(testCombTwo, testGenomeZero, 0, true);
    out << "Testing with 1 (i.e. 01 in binary) on zeros - this should hit 64 (2 in the interactions table). Fitness change is " << fitness << "\n";
    if (fitness != 64) testFlag = false;

    fitness = testInteractionSystem.performInteractFitness(testCombOne, testGenomeMax, 0, true);
    out << "Testing with 2 (i.e. 10 in binary) on ones - this should hit -64 then be corrected to zero. Fitness change is " << fitness << "\n";
    if (fitness != 0) testFlag = false;

    fitness = testInteractionSystem.performInteractFitness(testCombTwo, testGenomeMax, 0, true);
    out << "Testing with 1 (i.e. 01 in binary) on ones - this should hit -64 then be corrected to zero. Fitness change is " << fitness << "\n";
    if (fitness != 0) testFlag = false;

    out << "\nNew a priori interactions grid :\n";
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            simulationManager->simulationSettings->a_priori_interaction[i][j] = 1;
            out << simulationManager->simulationSettings->a_priori_interaction[i][j] << " ";
        }
        out << "\n";
    }

    simulationManager->makeInteractionLookups();

    fitness = testInteractionSystem.performInteractFitness(testGenomeZero, testGenomeZero, 0, true);
    out << "\nTesting with all zeros - fitness change should be thirty-two, is " << fitness << "\n";
    if (fitness != 32) testFlag = false;

    fitness = testInteractionSystem.performInteractFitness(testGenomeMax, testGenomeMax, 0, true);
    out << "Testing with all ones - fitness change should be thirty-two, is " << fitness << "\n";
    if (fitness != 32) testFlag = false;

    out << "Now testing with 100 random genomes, all should be 32 as well.\n\nResults:\n";

    for (int i = 0; i < 100; i++)
    {
        quint32 testGenomeRandom1[2] = {simulationManager->simulationRandoms->rand32()};
        quint32 testGenomeRandom2[2] = {simulationManager->simulationRandoms->rand32()};
        fitness = testInteractionSystem.performInteractFitness(testGenomeRandom1, testGenomeRandom2, 0, true);
        out << fitness << " ";
        if ((i + 1) % 20 == 0) out << "\n";
        if (fitness != 32) testFlag = false;
    }

    if (testFlag) out << "Fitness blocks tests passed.\n\n";

    //RJG - reset interactions lookups
    std::copy(&backup_interaction[0][0], &backup_interaction[0][0] + 16, &simulationManager->simulationSettings->a_priori_interaction[0][0]);
    simulationManager->makeInteractionLookups();

    //RJG - now test the genome XOR interactions
    out << "Now doing fitness XOR interactions.\n";
    fitness = testInteractionSystem.performInteractFitness(testGenomeZero, testGenomeZero, 0, false);
    out << "Testing with all zeros - fitness change should be zero, is " << fitness << "\n";
    if (fitness != 0) testFlag = false;

    fitness = testInteractionSystem.performInteractFitness(testGenomeMax, testGenomeMax, 0, false);
    out << "Testing with all ones - fitness change should be zero, is " << fitness << "\n";
    if (fitness != 0) testFlag = false;

    fitness = testInteractionSystem.performInteractFitness(testGenomeZero, testGenomeMax, 0, false);
    out << "Testing with all zeros on ones - fitness change should be zero (the leftwise and rightwise bitshifts cancel out), is " << fitness << "\n";
    if (fitness != 0) testFlag = false;

    fitness = testInteractionSystem.performInteractFitness(testGenomeMax, testGenomeZero, 0, false);
    out << "Testing with all zeros on ones - fitness change should be zero (the leftwise and rightwise bitshifts cancel out), is " << fitness << "\n";
    if (fitness != 0) testFlag = false;

    //Tested fitness, now need to test energy


    return testFlag;
}

bool test::testSeven(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    //This tests features available in development branch, disabled in main.
    /*
    out << "Testing save and load functions. This will save a file to your current output directory, overwriting any .revosim files already there, FYI.";

    //RJG - Environment system look ups / masks
    //RJG - create copy, then overwrite to check they reload correctly
    quint32 xorMasks[256][3] = {{0}};
    for (int i = 0; i < 256; i++)
        for (int j = 0; j < 3; j++)
            xorMasks[i][j] = simulationManager->environmentalFitnessSytem->returnXOR(i, j);

    QString filename(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/revosimTestFile.revosim");
    mainWindow->saveSimulation(filename);

    //RJG - Blank masks
    for (int i = 0; i < 256; i++)
        for (int j = 0; j < 3; j++)
            simulationManager->environmentalFitnessSytem->setXOR(i, j, 0);

    out << "\nMasks backup left, and set to zero (right):\n";
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 3; j++)
            out <<  xorMasks[i][j] << "\t" << simulationManager->environmentalFitnessSytem->returnXOR(i, j) << "\n";

    mainWindow->loadSimulation(filename);

    out << "Masks backup (left), and after load (right):\n";
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 3; j++)
        {
            out <<  xorMasks[i][j] << "\t" << simulationManager->environmentalFitnessSytem->returnXOR(i, j) << "\n";
            if (xorMasks[i][j] != simulationManager->environmentalFitnessSytem->returnXOR(i, j)) testFlag = false;
        }

    out << "\n\nTesting save and load of the main critters array.\n\n";
    mainWindow->toggleTestUI();
    mainWindow->resize();
    mainWindow->runForNSimulation(100);

    QHash<QString, Critter > crittersBackup;
    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
            for (int k = 0; k < simulationManager->cellSettings[i][j].slotsPerSquare; k++)
            {
                if (critters[i][j][k].age > 0)
                {
                    QString key;
                    key.append( QString::number(i));
                    key.append( QString::number(j));
                    key.append( QString::number(k));

                    Critter crit;
                    quint8 env[3] = {0, 0, 0};
                    crit.initialise(critters[i][j][k].genomeWords, env, i, j, k, critters[i][j][k].speciesID, (critters[i][j][k].trophicLevel * 100));
                    crit = critters[i][j][k];
                    crittersBackup.insert(key, crit);
                }
            }

    mainWindow->saveSimulation(filename);

    mainWindow->resetSimulation();
    mainWindow->resize();
    mainWindow->toggleTestUI();

    mainWindow->loadSimulation(filename);


    //To compare - check if key exists, and if so then compare with critter - else don't
    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
            for (int k = 0; k < simulationManager->cellSettings[i][j].slotsPerSquare; k++)
            {
                if (testFlag)
                {
                    {
                        QString key;
                        key.append( QString::number(i));
                        key.append( QString::number(j));
                        key.append( QString::number(k));
                        QHash<QString, Critter>::const_iterator it = crittersBackup.constFind(key);
                        bool notFound = false;
                        if (it == crittersBackup.constEnd()) notFound = true;
                        if (critters[i][j][k].age > 0 && notFound)
                        {
                            testFlag = false;
                            out << "Critter error: age >0 and not found.\n";
                        }
                        if (!notFound)
                        {
                            if (critters[i][j][k].age == 0)
                            {
                                testFlag = false;
                                out << "Critter error: found but age is zero.\n";
                                qDebug() << critters[i][j][k].fitness;
                            }
                            else if (!(critters[i][j][k] == it.value()))
                            {
                                testFlag = false;
                                out << "Critter error: critters are unequal";
                            }
                        }
                    }
                }
            }
    */

    return testFlag;
}

bool test::testEight(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing the HGT system with 100 known genomes.\n";

    // set settings for hgt transfer
    simulationManager->cellSettingsMaster->hgtTransferLength = 5;
    simulationManager->simulationSettings->hgtMode = HGT_SYNOYMOUS;
    simulationManager->simulationSettings->genomeSize = 1;
    out << "\n Settings: \n Genome transfer length = 5 \n HGT mode = Synonoumous\n\n";


    for (int i = 0; i < 100; i++)
    {
        out << "\n\n" << i << "\n";

        // create recipent and donor genomes
        quint32 rgenome = {simulationManager->simulationRandoms->rand32()};
        quint32 dgenome = {simulationManager->simulationRandoms->rand32()};


        // create initial mask of genome length
        //mask =  ~(0 << simulationManager->simulationSettings->genomeSize*32); //genomes of 1s
        // use generateMask function to create mask of transfer segment
        quint32 mask = simulationManager->hgtSystem->generateMask(&dgenome);

        //determine first set bit in mask
        int position = 0;
        int maskchk = mask;
        for (int i = 0; i < 32; i++)
        {

            int flag = (maskchk >> i ) & 1;
            if (flag == 1) {
                break;
            }
            position++;
        }

        // Test 1- Check mask has correct number of bits set
        int target = simulationManager->hgtSystem->bitCount(mask);
        if ( simulationManager->cellSettingsMaster->hgtTransferLength != target)
        {
            testFlag = false;
            out << "\n Mask has incorrect number of bits\n";
        }

        else out << "Mask has correct number of bits set... \n";



        //Test 2 - Check if the transfer sequence matches the donor genome after GenerateTransform
        quint32 genometransfer;
        genometransfer = simulationManager->hgtSystem->GenerateTransform(dgenome, mask);


        int transfercheck = (genometransfer >> position) & 1;
        int donorcheck = (dgenome >> position) & 1;
        if (transfercheck != donorcheck)
        {
            testFlag = false;
            out << "\n Mask does not contain binary sequence from donor genome in correct position \n";
        }
        else out << "Mask contains binary sequence from donor genome in correct position... \n";


        out << "\n Donor genome :------ " << simulationManager->printGenome(dgenome) ;
        out << "\n Recipient genome :-- " <<  simulationManager->printGenome(rgenome) << "\n" ;
        out << "--------------------------------------------------------------------------";


        //Test 3 - Apply the transformation and check if the recipient genome matches the donor in the transfer position
        simulationManager->hgtSystem->Transform(&rgenome, genometransfer, mask);
        out << "\n Recipient after :----- " <<  simulationManager->printGenome(rgenome) << "\n" ;
        out << "Mask :------------------" <<  simulationManager->printGenome(mask) << "\n" ;

        transfercheck = (genometransfer >> position) & 1;
        donorcheck = (dgenome >> position) & 1;
        if (transfercheck != donorcheck)
        {
            testFlag = false;
            out << "\n Transformation unsucessful \n";
        }
        else out << "\n Transformation sucessful... \n";

        //out << "Mask  : \n " << simulationManager->printGenome(rgenome);
    }
    if (testFlag) out << "\n Tests passed.\n\n";
    return testFlag;
}

bool test::testNine(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testTen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testEleven(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testTwelve(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testThirteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testFourteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testFifteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testSixteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testSeventeen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testEighteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testNineteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testTwenty(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

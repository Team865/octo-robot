#include <gtest/gtest.h>

#include "../firmware_v1/util_profile.h"
#include "test_mock_net.h"

namespace Util {

#ifdef TODO
TEST( profile_should, and_and_report_properly )
{
  Profile test("test");
  test.addSample( Time::TimeUS( 10 ));
  test.addSample( Time::TimeUS( 20 ));
  test.addSample( Time::TimeUS( 20 ));
  test.addSample( Time::TimeUS( 95 ));
  test.addSample( Time::TimeUS( 430 ));

  NetMockSimpleTimed sink;
  test.reportHistogram( sink );

  TimedStringEvents goldenNet = {
    { 0, "Histogram for test" },
    { 0, " 60  0uS  -> 23uS" },
    { 0, "  0  24uS  -> 47uS" },
    { 0, "  0  48uS  -> 71uS" },
    { 0, " 20  72uS  -> 95uS" },
    { 0, "  0  96uS  -> 119uS" },
    { 0, "  0  120uS  -> 143uS" },
    { 0, "  0  144uS  -> 167uS" },
    { 0, "  0  168uS  -> 191uS" },
    { 0, "  0  192uS  -> 215uS" },
    { 0, "  0  216uS  -> 239uS" },
    { 0, "  0  240uS  -> 263uS" },
    { 0, "  0  264uS  -> 287uS" },
    { 0, "  0  288uS  -> 311uS" },
    { 0, "  0  312uS  -> 335uS" },
    { 0, "  0  336uS  -> 359uS" },
    { 0, "  0  360uS  -> 383uS" },
    { 0, "  0  384uS  -> 407uS" },
    { 0, " 20  408uS  -> 431uS" },
    { 0, "  0  432uS  -> 455uS" },
    { 0, "  0  456uS  -> 479uS" }
  };

  ASSERT_EQ( goldenNet, sink.getOutput() );

  NetMockSimpleTimed sink2;
  test.reportOneLiner( sink2 );
  TimedStringEvents goldenNet2 = {
    { 0, "test                 50% = 23uS   90% = 431uS   98% = 431uS   max = 431uS" }
  };
  ASSERT_EQ( goldenNet2, sink2.getOutput() );

  Profile empty("empty");

  NetMockSimpleTimed sink3;
  empty.reportOneLiner( sink3 );
  TimedStringEvents goldenNet3 = {
    { 0, "empty                No Samples" }
  };
  ASSERT_EQ( goldenNet3, sink3.getOutput() );

  NetMockSimpleTimed sink4;
  empty.reportHistogram( sink4 );
  TimedStringEvents goldenNet4 = {
    { 0, "Histogram for empty" },
    { 0, "  No Samples" }
  };
  ASSERT_EQ( goldenNet4, sink4.getOutput() );
}
#endif

} // end Action namespace


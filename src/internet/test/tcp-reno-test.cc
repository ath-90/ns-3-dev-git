/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Athreya H N <indiathreya92@gmail.com>
 *          Anubhav Jain <anubhav.13jain@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#include "ns3/log.h"
#include "tcp-general-test.h"
#include "ns3/simple-channel.h"
#include "ns3/node.h"
#include "tcp-error-model.h"
#include "ns3/tcp-reno.h"
#include <list>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpRenoTestSuite");

/**
 * \ingroup internet-test
 * \ingroup tests
 *
 * \brief Test TCP Reno
 *
 */

class TcpRenoTest : public TcpGeneralTest
{
public:
  /**
   * \brief Constructor
   * \param congControl Type of congestion control.
   * \param seqsToKill List of Sequence number of the packets to drop.
   * \param msg Test message.
   */

  TcpRenoTest (TypeId congControl, std::list <uint32_t> seqsToKill, const std::string &msg);

  virtual Ptr <ErrorModel> CreateSenderErrorModel ();

  virtual Ptr <ErrorModel> CreateReceiverErrorModel ();

  virtual Ptr <TcpSocketMsgBase> CreateSenderSocket (Ptr <Node> node);

protected:
  virtual void CongStateTrace (const TcpSocketState::TcpCongState_t oldValue,
                               const TcpSocketState::TcpCongState_t newValue);

  /**
   * \brief Check if the packet being dropped is the right one.
   * \param ipH IPv4 header.
   * \param tcpH TCP header.
   * \param p The packet.
   */
  void PktDropped (const Ipv4Header &ipH, const TcpHeader &tcpH, Ptr<const Packet> p);

  virtual void FinalChecks ();

  virtual void ConfigureProperties ();

  virtual void ConfigureEnvironment ();

  std::list <uint32_t> m_seqsToKill;   //!< Sequence numbers to drop.
  uint32_t m_countReco;                //!< Number of times the program enters recovery.
  Ptr <TcpSeqErrorModel> m_errorModel; //!< Error model.
};

TcpRenoTest::TcpRenoTest (TypeId typeId, std::list <uint32_t> seqsToKill,
                          const std::string &msg)
  : TcpGeneralTest (msg),
  m_seqsToKill (seqsToKill),
  m_countReco (0)
{
  m_congControlTypeId = typeId;
}

void
TcpRenoTest::ConfigureProperties ()
{
  TcpGeneralTest::ConfigureProperties ();
  SetInitialCwnd (SENDER, 10);
}

void
TcpRenoTest::ConfigureEnvironment ()
{
  TcpGeneralTest::ConfigureEnvironment ();
  SetAppPktCount (200);
}

Ptr <ErrorModel>
TcpRenoTest::CreateSenderErrorModel ()
{
  return 0;
}

Ptr <ErrorModel>
TcpRenoTest::CreateReceiverErrorModel ()
{
  m_errorModel = CreateObject<TcpSeqErrorModel> ();

  for (std::list<uint32_t>::iterator seq = m_seqsToKill.begin (); seq != m_seqsToKill.end (); ++seq)
    {
      m_errorModel->AddSeqToKill (SequenceNumber32 (*seq));
    }

  m_errorModel->SetDropCallback (MakeCallback (&TcpRenoTest::PktDropped, this));

  return m_errorModel;
}


Ptr <TcpSocketMsgBase>
TcpRenoTest::CreateSenderSocket (Ptr <Node> node)
{
  Ptr <TcpSocketMsgBase> socket = TcpGeneralTest::CreateSenderSocket (node);
  socket->SetAttribute ("MinRto", TimeValue (Seconds (10.0)));
  socket->SetAttribute ("Sack", BooleanValue (false));

  return socket;
}

void
TcpRenoTest::CongStateTrace (const TcpSocketState::TcpCongState_t oldValue,
                             const TcpSocketState::TcpCongState_t newValue)
{
  NS_LOG_FUNCTION (this << oldValue << newValue);

  if (oldValue == TcpSocketState::CA_DISORDER
      && newValue == TcpSocketState::CA_RECOVERY)
    {
      ++m_countReco;
    }
}


void
TcpRenoTest::PktDropped (const Ipv4Header &ipH, const TcpHeader &tcpH, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (this << ipH << tcpH);

  int seq_match_count = 0;
  for (std::list<uint32_t>::iterator seq = m_seqsToKill.begin (); seq != m_seqsToKill.end (); ++seq)
    {
      if (tcpH.GetSequenceNumber () == SequenceNumber32 (*seq))
        {
          ++seq_match_count;
          break;
        }
    }

  if (seq_match_count == 0)
    {
      NS_TEST_ASSERT_MSG_EQ (true, false,
                             "Packet dropped but sequence number differs");
    }
}

void
TcpRenoTest::FinalChecks ()
{
  NS_TEST_ASSERT_MSG_EQ (m_seqsToKill.size (), m_countReco,
                         "Number of drops does not match the number of times the algorithm entered recovery");
}

/**
 * \ingroup internet-test
 * \ingroup tests
 *
 * \brief Testsuite for the TCP Reno
 */
class TcpRenoTestSuite : public TestSuite
{
public:
  TcpRenoTestSuite () : TestSuite ("tcp-reno-test", UNIT)
  {
    std::list <uint32_t> seqsToKill;

    seqsToKill.push_back (7001);
    seqsToKill.push_back (7501);
    seqsToKill.push_back (8001);

    AddTestCase (new TcpRenoTest (TcpReno::GetTypeId (), seqsToKill, "TCP Reno testing"), TestCase::QUICK);
  }
};

static TcpRenoTestSuite g_TcpRenoTestSuite; //!< Static variable for test initialization
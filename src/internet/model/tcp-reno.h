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

#ifndef TCP_RENO_H
#define TCP_RENO_H

#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/tcp-socket-state.h"

namespace ns3 {

class TcpReno : public TcpCongestionOps
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  TcpReno ();

  /**
   * \brief Copy constructor.
   * \param sock object to copy.
   */
  TcpReno (const TcpReno &sock);

  ~TcpReno ();

  std::string GetName () const;

  virtual void IncreaseWindow (Ptr <TcpSocketState> tcb, uint32_t segmentsAcked);

  virtual uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb,
                                uint32_t bytesInFlight);

  virtual Ptr <TcpCongestionOps> Fork ();

protected:
  virtual uint32_t SlowStart (Ptr <TcpSocketState> tcb, uint32_t segmentsAcked);

  virtual void CongestionAvoidance (Ptr <TcpSocketState> tcb, uint32_t segmentsAcked);
};
}

#endif // TCPRENO_H
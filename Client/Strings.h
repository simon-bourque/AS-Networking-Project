#pragma once

static char constexpr s_mainMenuString[] = "\n0.Register\n1.Deregister\n2.Send Offer\n3.Send Bid\n4.Display Offers\n5.Display Bids\n6.Display Won Items\n7.Disconnect";
static char constexpr s_separator[] = "=============================================================";

// Errors
static char constexpr s_notReg[] = "[ERROR] User not registered";
static char constexpr s_alreadyReg[] = "[ERROR] User already registered";
static char constexpr s_serverErrorUDP[] = "[ERROR] Server error during UDP watch";
static char constexpr s_serverErrorTCP[] = "[ERROR] Server error during TCP watch";
static char constexpr s_tcpError[] = "[ERROR] An error occured while receiving a TCP packet";
static char constexpr s_wrongAck[]= "[ERROR] Received acknowledgement of wrong request";
static char constexpr s_notAck[] = "[ERROR] Sent packet was not acknowledged";
static char constexpr s_tcpForceClosed[] = "\n[ERROR] Remote connection was forcibly closed. You are now unregistered.";

// Info
static char constexpr s_tcpClosed[] = "[INFO] TCP Connection was gracefully closed.";
static char constexpr s_bidNotFound[] = "[INFO] Previous bid was not found locally. Ignoring.";

// Update
static char constexpr s_highest[] = "[UPDATE] One of your bids has been surpassed";
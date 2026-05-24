// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayTagMessageDefines.h"

// --- Channels

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Channels,			"Channels", "Channels that GameplayMessages can be transmitted through.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Channels_Combat,		"Channels.Combat", "For Combat events.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Channels_Combat_Enemy,		"Channels.Combat.Enemy", "Enemy-specific Combat events.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Channels_Combat_Player,		"Channels.Combat.Player", "Player-specific Combat events.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Channels_Combat_DamageEvents,	"Channels.Combat.DamageEvent", "Any generated DamageEvents from combat are sent in this channel. Expects FAADamageEventMessage.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Channels_Combat_Rewards,		"Channels.Combat.Rewards", "Rewards earned from the destruction or death of an object. Might or might not be rewarded to the player. Expects FAARewardMessage!");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Channels_GameplayEvents,		"Channels.GameplayEvents", "Gameplay events.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Channels_GameplayEvents_Tutorial, "Channels.GameplayEvents.Tutorial", "Tutorial events.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Channels_GameplayEvents_Player,	"Channels.GameplayEvents.Player", "Player-specific Gameplay events.");


// --- Verbs

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs,			"Verbs", "The action or event that triggered this message.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_EnteredCombat,		"Verbs.EnteredCombat", "Target entered combat. Only triggered once until exiting combat again.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_ExitedCombat,		"Verbs.ExitedCombat", "Target exited combat. Only triggered once until entering combat again.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_AggroedOn,			"Verbs.AggroedOn", "Instigator aggroed on Target.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_Hit,				"Verbs.Hit", "Instigator hit Target with an attack.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_Spawned,			"Verbs.Spawned", "Target spawned.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_StopTargetLock,	"Verbs.StopTargetLock", "Request from Instigator that any TargetLocking should cease on it.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_ActivatedAction,	"Verbs.ActivatedAction", "An action (often GameplayAbility) of some type has activated.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_ActivatedAction_LightAttack,			"Verbs.ActivatedAction.LightAttack", "");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_ActivatedAction_HeavyAttack,			"Verbs.ActivatedAction.HeavyAttack", "");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_ActivatedAction_ChargedHeavyAttack,	"Verbs.ActivatedAction.ChargedHeavyAttack", "");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_ActivatedAction_ModeSwitch,			"Verbs.ActivatedAction.ModeSwitch", "");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_ActivatedAction_ModeSwitch_Normal,		"Verbs.ActivatedAction.ModeSwitch.Normal",  "Normal Mode.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_ActivatedAction_ModeSwitch_Infrared, "Verbs.ActivatedAction.ModeSwitch.Infrared", "Infrared Mode.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_ActivatedAction_ModeSwitch_Ultraviolet, "Verbs.ActivatedAction.ModeSwitch.Ultraviolet", "Ultraviolet Mode.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Verbs_ActivatedAction_ModeSwitch_Gamma, "Verbs.ActivatedAction.ModeSwitch.Gamma", "Gamma Mode.");

// --- Contexts

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Contexts,				"Contexts", "Gives context. Used as general context tags and not only for GameplayMessages");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Contexts_Global,			"Contexts.Global",	"Activation or usage is global.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GAMEPLAYTAG_Contexts_Source,			"Contexts.Source",	"Activation or usage is source specific.");

/*
 * HTrigger.cpp
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#include "HTrigger.h"

#include "../interfaces.h"
#include "../drawing.h"
#include "../usercmd.h"
#include "../logging.h"
#include "../entity.h"
#include "../enums.h"
#include "../helpers.h"
#include "../trace.h"
#include "../localplayer.h"

#include "../fixsdk.h"
#include "../sdk/in_buttons.h"
#include <icliententity.h>
#include <cdll_int.h>
#include <icliententitylist.h>
#include <engine/IEngineTrace.h>
#include <Color.h>
#include <gametrace.h>
#include <icliententity.h>
#include <client_class.h>
#include <tier1/convar.h>

Vector eye;
trace_t* enemy_trace;
trace::FilterDefault* filter;

void HTrigger::Create() {
	filter = new trace::FilterDefault();
	enemy_trace = new trace_t();
	this->v_bBodyshot = CreateConVar("u_trigger_bodyshot", "1", "Enables bodyshotting when there is enough charge to oneshot enemy");
	this->v_bEnabled = CreateConVar("u_trigger_enabled", "0", "Triggerbot enabled");
	this->v_bFinishingHit = CreateConVar("u_trigger_finish", "1", "Allows noscope bodyshots when enemy is at <50 health");
	this->v_bIgnoreCloak = CreateConVar("u_trigger_cloak", "0", "Gets triggered at cloaked spies");
	this->v_bZoomedOnly = CreateConVar("u_trigger_zoomed", "1", "Trigger is only active when you are zoomed (as sniper)");
	this->v_iHitbox = CreateConVar("u_trigger_hitbox", "-1", "Hitbox (-1: whole body)");
	this->v_iMinRange = CreateConVar("u_trigger_range", "0", "Trigger is activated only at certain range");
	this->v_bBuildings = CreateConVar("u_trigger_buildings", "1", "Trigger is activated at buildings");
}

bool HTrigger::CreateMove(void* thisptr, float sampl, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	if (g_pLocalPlayer->life_state) return true;
	/*IClientEntity* local = interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer());
	if (!local) return;
	if (GetEntityValue<char>(local, entityvars.iLifeState)) return;*/
	Ray_t ray;
	filter->SetSelf(g_pLocalPlayer->entity);
	eye = g_pLocalPlayer->v_Eye;
	Vector forward;
	float sp, sy, cp, cy;
	sy = sinf(DEG2RAD(cmd->viewangles[1])); // yaw
	cy = cosf(DEG2RAD(cmd->viewangles[1]));

	sp = sinf(DEG2RAD(cmd->viewangles[0])); // pitch
	cp = cosf(DEG2RAD(cmd->viewangles[0]));

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
	forward = forward * 8192.0f + eye;
	ray.Init(eye, forward);
	interfaces::trace->TraceRay(ray, 0x4200400B, filter, enemy_trace);
	IClientEntity* entity = (IClientEntity*)(enemy_trace->m_pEnt);
	if (!entity) return true;
	bool isPlayer = false;
	switch (entity->GetClientClass()->m_ClassID) {
	case 241:
		isPlayer = true;
	break;
	case 89:
	case 88:
	case 86:
		if (!this->v_bBuildings->GetBool()) {
			return true;
		}
	break;
	default:
	return true;
	}; // TODO magic number
	int team = g_pLocalPlayer->team;
	int eteam = GetEntityValue<int>(entity, eoffsets.iTeamNum);
	if (team == eteam) return true;
	Vector enemy_pos = entity->GetAbsOrigin();
	Vector my_pos = g_pLocalPlayer->entity->GetAbsOrigin();
	if (v_iMinRange->GetInt() > 0) {
		if ((enemy_pos - my_pos).Length() > v_iMinRange->GetInt()) return true;
	}
	if (!isPlayer) {
		cmd->buttons |= IN_ATTACK;
		return true;
	}
	if (GetRelation(entity) == relation::FRIEND) return true;
	if (IsPlayerInvulnerable(entity)) return true;
	if (!this->v_bIgnoreCloak->GetBool() &&
		((GetEntityValue<int>(entity, eoffsets.iCond)) & cond::cloaked)) return true;
	int health = GetEntityValue<int>(entity, eoffsets.iHealth);
	bool bodyshot = false;
	if (g_pLocalPlayer->clazz == tf_class::tf_sniper) {
		// If sniper..
		if (health <= 50 && this->v_bFinishingHit->GetBool()) {
			bodyshot = true;
		}
		// If we need charge...
		if (!bodyshot && this->v_bBodyshot->GetBool()) {
			float bdmg = GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.flChargedDamage);
			if (bdmg >= 15.0f && (bdmg) >= health) {
				bodyshot = true;
			}
		}

	}
	if (!bodyshot && (g_pLocalPlayer->clazz == tf_class::tf_sniper) && this->v_bZoomedOnly->GetBool() &&
		!(g_pLocalPlayer->cond_0 & cond::zoomed)) {
		return true;
	}
	//IClientEntity* weapon;
	if (this->v_iHitbox->GetInt() >= 0 && !bodyshot) {
		if (enemy_trace->hitbox != this->v_iHitbox->GetInt()) return true;
	}
	cmd->buttons |= IN_ATTACK;
	return true;
}

void HTrigger::Destroy() {
	delete filter;
	delete enemy_trace;
}

void HTrigger::PaintTraverse(void*, unsigned int, bool, bool) {};

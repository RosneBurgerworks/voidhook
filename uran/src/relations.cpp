/*
 * relations.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: nullifiedcat
 */

#include "relations.h"
#include "logging.h"



void CRelations::Add(uint32 id, relation rel) {
	logging::Info("Adding");
}

void CRelations::Remove(uint32 id) {
	for (int i = 0; i < m_nFriends; i++) {
		if (m_Friends[i] == id) {
			m_nFriends--;
			ShiftArray(m_Friends, i, m_nFriends);
		}
	}
	for (int i = 0; i < m_nFriendlies; i++) {
		if (m_Friendlies[i] == id) {
			m_nFriendlies--;
			ShiftArray(m_Friendlies, i, m_nFriendlies);
		}
	}
	for (int i = 0; i < m_nRage; i++) {
		if (m_Rage[i] == id) {
			m_nRage--;
			ShiftArray(m_Rage, i, m_nRage);
		}
	}
}

void CRelations::WriteConfig() {
	FILE* cfg = fopen("", "w");

	fclose(cfg);
}

void CRelations::ShiftArray(uint32* array, int idx, int max) {
	for (int i = idx; i < max; i++) {
		array[idx] = array[idx + 1];
	}
}

char* RelStr[] = { "NONE", "FRIEND", "FRIENDLY", "RAGE" };
CRelations* g_pRelations = 0;

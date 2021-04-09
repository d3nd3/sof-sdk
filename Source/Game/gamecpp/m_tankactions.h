/**********************************************************************************
 **********************************************************************************/

class tank_action : public action_c
{
private:
protected:
	int		m_nID;
	int		m_nCommand;
	float	m_fArg;

	void	(*m_think)(edict_t *self);
	void	(*m_nextthink)(edict_t *self);

	virtual qboolean	SetAnimation(ai_c &which_ai, edict_t &monster);
	virtual qboolean	ForceAnimation(ai_c &which_ai, edict_t &monster);

public:

	tank_action(decision_c *od, action_c *oa, ai_c* which_ai, mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target,
				float fArg, int nID);
	tank_action(void):action_c(){ m_nID = -1;}
	virtual ~tank_action(void) {}

	virtual int		GetClassCode(void){return TANK_ACTION;}//this should be different for everybody in this class family

	virtual actiontype_e Type(void){return atype_walk;}
	virtual qboolean	Think(ai_c &which_ai, edict_t &monster);
	virtual void SetThink(void	(*think)(edict_t *self)) { m_think = think; }
	virtual void SetNextThink(void	(*think)(edict_t *self)) { m_nextthink = think; }

			tank_action(tank_action *orig);
			void Evaluate(tank_action *orig);
	virtual void Write(void);
	virtual void Read(void);

	virtual int GetID() { return m_nID; }
};


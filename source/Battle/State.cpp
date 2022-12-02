#include "State.h"
#include "Actors/PlayerCharacter.h"

void ScriptState::OnEnter()
{
    if (ParentState)
    {
        ParentState->OnEnter();
    }
    if (Offsets.OnEnterOffset == -1)
        return;
    if (CommonState)
    {
        Parent->CommonAnalyzer->Analyze((char*)Offsets.OnEnterOffset, Parent);
        return;
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnEnterOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnEnterOffset, Parent);
    }
}

void ScriptState::OnUpdate(float DeltaTime)
{
    if (ParentState)
    {
        ParentState->OnUpdate(DeltaTime);
    }
    if (Offsets.OnUpdateOffset == -1)
        return;
    if (CommonState)
    {
        Parent->CommonAnalyzer->Analyze((char*)Offsets.OnUpdateOffset, Parent);
        return;
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnUpdateOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnUpdateOffset, Parent);
    }
}

void ScriptState::OnExit()
{
    if (ParentState)
    {
        ParentState->OnExit();
    }
    if (Offsets.OnExitOffset == -1)
        return;
    if (CommonState)
    {
        Parent->CommonAnalyzer->Analyze((char*)Offsets.OnExitOffset, Parent);
        return;
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnExitOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnExitOffset, Parent);
    }
}

void ScriptState::OnLanding()
{
    if (ParentState)
    {
        ParentState->OnLanding();
    }
    if (Offsets.OnLandingOffset == -1)
        return;
    if (CommonState)
    {
        Parent->CommonAnalyzer->Analyze((char*)Offsets.OnLandingOffset, Parent);
        return;
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnLandingOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnLandingOffset, Parent);
    }
}

void ScriptState::OnHit()
{
    if (ParentState)
    {
        ParentState->OnHit();
    }
    if (Offsets.OnHitOffset == -1)
        return;
    if (CommonState)
    {
        Parent->CommonAnalyzer->Analyze((char*)Offsets.OnHitOffset, Parent);
        return;
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnHitOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnHitOffset, Parent);
    }
}

void ScriptState::OnBlock()
{
    if (ParentState)
    {
        ParentState->OnBlock();
    }
    if (Offsets.OnBlockOffset == -1)
        return;
    if (CommonState)
    {
        Parent->CommonAnalyzer->Analyze((char*)Offsets.OnBlockOffset, Parent);
        return;
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnBlockOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnBlockOffset, Parent);
    }
}

void ScriptState::OnHitOrBlock()
{
    if (ParentState)
    {
        ParentState->OnHitOrBlock();
    }
    if (Offsets.OnHitOrBlockOffset == -1)
        return;
    if (CommonState)
    {
        Parent->CommonAnalyzer->Analyze((char*)Offsets.OnHitOrBlockOffset, Parent);
        return;
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnHitOrBlockOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnHitOrBlockOffset, Parent);
    }
}

void ScriptState::OnCounterHit()
{
    if (ParentState)
    {
        ParentState->OnCounterHit();
    }
    if (Offsets.OnCounterHitOffset == -1)
        return;
    if (CommonState)
    {
        Parent->CommonAnalyzer->Analyze((char*)Offsets.OnCounterHitOffset, Parent);
        return;
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnCounterHitOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnCounterHitOffset, Parent);
    }
}

void ScriptState::OnSuperFreeze()
{
    if (ParentState)
    {
        ParentState->OnSuperFreeze();
    }
    if (Offsets.OnSuperFreezeOffset == -1)
        return;
    if (CommonState)
    {
        Parent->CommonAnalyzer->Analyze((char*)Offsets.OnSuperFreezeOffset, Parent);
        return;
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnSuperFreezeOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnSuperFreezeOffset, Parent);
    }
}

void ScriptState::OnSuperFreezeEnd()
{
    if (ParentState)
    {
        ParentState->OnSuperFreezeEnd();
    }
    if (Offsets.OnSuperFreezeEndOffset == -1)
        return;
    if (CommonState)
    {
        Parent->CommonAnalyzer->Analyze((char*)Offsets.OnSuperFreezeEndOffset, Parent);
        return;
    }
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnSuperFreezeEndOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnSuperFreezeEndOffset, Parent);
    }
}
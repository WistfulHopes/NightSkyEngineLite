#include "State.h"
#include "Actors/PlayerCharacter.h"

void ScriptState::OnEnter()
{
    if (ParentState)
    {
        ParentState->OnEnter();
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
    if (Parent)
    {
        Parent->CharaAnalyzer->Analyze((char*)Offsets.OnEnterOffset, Parent);
    }
    else
    {
        ObjectParent->Player->CharaAnalyzer->Analyze((char*)Offsets.OnEnterOffset, Parent);
    }
}

void ScriptState::OnExit()
{
    if (ParentState)
    {
        ParentState->OnExit();
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

void ScriptState::OnLanding()
{
    if (ParentState)
    {
        ParentState->OnLanding();
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

void ScriptState::OnHit()
{
    if (ParentState)
    {
        ParentState->OnHit();
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

void ScriptState::OnBlock()
{
    if (ParentState)
    {
        ParentState->OnBlock();
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

void ScriptState::OnHitOrBlock()
{
    if (ParentState)
    {
        ParentState->OnHitOrBlock();
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

void ScriptState::OnCounterHit()
{
    if (ParentState)
    {
        ParentState->OnCounterHit();
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

void ScriptState::OnSuperFreeze()
{
    if (ParentState)
    {
        ParentState->OnSuperFreeze();
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

void ScriptState::OnSuperFreezeEnd()
{
    if (ParentState)
    {
        ParentState->OnSuperFreezeEnd();
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
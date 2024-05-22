#include "PrecompileHeader.h"
#include "EnemyState_WalkBase.h"

#include <queue>

#include "BackGround.h"
#include "FieldPlayer.h"
#include "FieldEnemyBase.h"

struct BestRouth
{
	int TotalPoint = INT32_MAX;
	int Cost = INT32_MAX;
	std::pair<int, int> Pos;	//X, Y

	//최소힙
	bool operator <(const BestRouth& _Other) const
	{
		return TotalPoint > _Other.TotalPoint;
	}
};

void EnemyState_WalkBase::FindPath()
{
	FieldEnemyBase* EnemyPtr = GetEnemy();

	//이 몬스터의 현재위치와 플레이어의 현재위치
	const std::pair<int, int> StartPos = EnemyPtr->GetGridPos();
	const std::pair<int, int>& DestPos = FieldPlayer::GetPtr()->GetGridPos();


	static std::map<std::pair<int, int>, std::pair<int, int>> Parent;
	Parent.clear();

	static std::map<std::pair<int, int>, int> OpenList;	//<위치, 비용>
	static std::set<std::pair<int, int>> CloseList;
	OpenList.clear();
	CloseList.clear();

	
	std::priority_queue<BestRouth> PQ;
	int StartCost = 10 * (abs(StartPos.first - DestPos.first) + abs(StartPos.second - DestPos.second));	


	OpenList[StartPos] = StartCost;
	Parent[StartPos] = StartPos;
	PQ.push({ StartCost, 0, StartPos });


	while (false == PQ.empty())
	{
		BestRouth Now = PQ.top();
		int X = Now.Pos.first;
		int Y = Now.Pos.second;
		PQ.pop();

		if (CloseList.end() != CloseList.find(Now.Pos))
			continue;

		CloseList.insert(Now.Pos);
		if (DestPos == Now.Pos)
			break;

		static const int Dx[] = { 1, 0, -1, 0, 1, 1, -1, -1 };
		static const int Dy[] = { 0, 1, 0, -1, 1, -1, 1, -1 };
		static const int NeedCost[] = { 10, 10, 10, 10, 14, 14, 14, 14 };

		for (size_t i = 0; i < 8; ++i)
		{
			int NextX = Now.Pos.first + Dx[i];
			int NextY = Now.Pos.second + Dy[i];

			//그리드 영역 밖일때
			if (NextX < 0 || NextY < 0 || GridMapScale.first < NextX || GridMapScale.second < NextY)
				continue;

			//막힌 곳일때
			if (true == GetBackGround()->IsBlockGrid(NextX, NextY))
				continue;

			//이미 방문한 곳일때
			std::pair<int, int> NextPos = std::pair<int, int>(NextX, NextY);
			if (CloseList.end() != CloseList.find(NextPos))
				continue;


			int NextCost = Now.Cost + NeedCost[i];
			int Distance = 10 * (abs(DestPos.first - NextX) + abs(DestPos.second - NextY));
			int NextTotalPoint = NextCost + Distance;

			//이전에는 예약하지 않았던 위치인 경우
			if (false == OpenList.contains(NextPos))
			{
				OpenList[NextPos] = INT32_MAX;
			}

			int& PrevCost = OpenList[NextPos];
			if (PrevCost < NextTotalPoint)
				continue;

			PrevCost = NextTotalPoint;
			PQ.push({ NextTotalPoint, NextCost,NextPos });
			Parent[NextPos] = Now.Pos;
		}
	}


	PathStack.clear();
	if (PathStack.capacity() < CloseList.size())
	{
		PathStack.reserve(CloseList.size());
	}
	

	//예외처리, 몬스터가 플레이어에게 이동할 수 없는 위치인 경우 ASSERT_CRASH
	if (false == Parent.contains(DestPos))
	{
		std::string StartFieldPosStr = EnemyPtr->GetTransform()->GetWorldPosition().ToString();
		std::string StartGridPosStr = "(" + std::to_string(StartPos.first) + ", " + std::to_string(StartPos.second) + ")";
		std::string SendText = "Enemy가 플레이어에게 이동할 수 없는 위치에 있습니다\n";
		SendText += "적의 시작위치 : " + StartFieldPosStr + "\n";
		SendText += "적의 시작 그리드 위치 : " + StartGridPosStr;
		MsgAssert(SendText);
		return;
	}

	

	std::map<std::pair<int, int>, std::pair<int, int>>::iterator FindIter = Parent.find(DestPos);
	std::pair<int, int> NextPos = FindIter->first;
	std::pair<int, int> PrevPos = FindIter->second;
	PathStack.push_back(NextPos);

	

	//처음 시작위치에 도달할때 까지 탐색 경로를 역으로 추적
	while (NextPos != PrevPos)
	{
		FindIter = Parent.find(PrevPos);
		NextPos = FindIter->first;
		PrevPos = FindIter->second;

		PathStack.push_back(NextPos);
	}
}

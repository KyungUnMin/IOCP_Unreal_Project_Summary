#pragma once


template<typename ResourcesType>
class GameEngineResource
{
public:
	GameEngineResource() {}
	virtual ~GameEngineResource() = 0 {}

	GameEngineResource(const GameEngineResource& _Other) = delete;
	GameEngineResource(GameEngineResource&& _Other) noexcept = delete;
	GameEngineResource& operator=(const GameEngineResource& _Other) = delete;
	GameEngineResource& operator=(GameEngineResource&& _Other) noexcept = delete;

	static std::shared_ptr<ResourcesType> Find(const std::string_view& _Name)
	{
		std::string UpperName = GameEngineString::ToUpper(_Name);

		std::lock_guard<std::mutex> Lock(NameLock);
		if (NamedResources.end() == NamedResources.find(UpperName.c_str()))
		{
			return nullptr;
		}

		return NamedResources[UpperName];
	}

protected:
	static std::shared_ptr<ResourcesType> Create(const std::string_view& _Name)
	{
		std::string UpperName = GameEngineString::ToUpper(_Name);
		if (nullptr != Find(UpperName))
		{
			MsgAssert("이미 존재하는 이름의 리소스를 또 만들려고 했습니다.");
			return nullptr;
		}

		std::shared_ptr<ResourcesType> NewRes = std::make_shared<ResourcesType>();
		NewRes->SetName(UpperName);

		std::lock_guard<std::mutex> Lock(NameLock);
		NamedResources.insert(std::make_pair(UpperName, NewRes));
		return NewRes;
	}

private:
	static std::map<std::string, std::shared_ptr<ResourcesType>> NamedResources;
	static std::mutex NameLock;
};

template<typename ResourcesType>
std::map<std::string, std::shared_ptr<ResourcesType>> GameEngineResource<ResourcesType>::NamedResources;

template<typename ResourcesType>
std::mutex GameEngineResource<ResourcesType>::NameLock;

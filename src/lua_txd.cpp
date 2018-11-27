// MIT License

// Copyright (c) 2012 DK22Pac
// Copyright (c) 2017 FYP

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "pch.h"
#include "lua_txd.h"
#include "lua_texture.h"
#include "game_sa/CTxdStore.h"


namespace lua_txd
{
	int getTxdId(const std::string& name)
	{
		return CTxdStore::FindTxdSlot(name.c_str());
	}

	sol::optional<std::shared_ptr<lua_texture::Texture>> findTexture(int txdId, const std::string& textureName)
	{
		if (txdId != -1)
		{
			if (!CTxdStore::ms_pTxdPool->m_byteMap[txdId].bEmpty)
			{
				auto dictionary = CTxdStore::ms_pTxdPool->m_pObjects[txdId].m_pRwDictionary;
				auto texture = RwTexDictionaryFindNamedTexture(dictionary, textureName.c_str());
				if (texture)
				{
					return std::make_shared<lua_texture::Texture>(texture, false);
				}
			}
		}
		return sol::nullopt;
	}

	class Txd
	{
	public:
		Txd(RwTexDictionary* txd, int id, const std::string& name, bool allocated = true) :
			_txd(txd),
			_id(id),
			_name(name),
			_allocated(allocated)
		{
		}
		~Txd()
		{
			if (_id != -1)
			{
				if (_allocated && !CTxdStore::ms_pTxdPool->IsFreeSlotAtIndex(_id))
					CTxdStore::RemoveRef(_id);
			}
		}

		const std::string& getName() const { return _name;  }
		int getId() const { return _id; }
		RwTexDictionary* getRwDictionary() const { return _txd; }
		uintptr_t getRawPointer() const { return reinterpret_cast<uintptr_t>(_txd); }

		sol::optional<std::shared_ptr<lua_texture::Texture>> getTexture(const std::string& name) const
		{
			if (!CTxdStore::ms_pTxdPool->IsFreeSlotAtIndex(_id))
			{
				auto dictionary = CTxdStore::ms_pTxdPool->m_pObjects[_id].m_pRwDictionary;
				auto texture = RwTexDictionaryFindNamedTexture(dictionary, name.c_str());
				if (texture)
				{
					return std::make_shared<lua_texture::Texture>(texture, false);
				}
			}
			return sol::nullopt;
		}

	private:
		RwTexDictionary* _txd;
		int _id;
		std::string _name;
		bool _allocated;
	};

	sol::optional<Txd> loadTxdFromFile(const std::string& path, const std::string& name)
	{
		int id = CTxdStore::AddTxdSlot(name.c_str());
		if (!CTxdStore::LoadTxd(id, path.c_str()))
		{
			CTxdStore::RemoveTxdSlot(id);
			return sol::nullopt;
		}
		auto txd = CTxdStore::AddRef(id);
		return Txd{txd->m_pRwDictionary, id, name};
	}

	sol::optional<Txd> getTxdFromId(int id)
	{
		if (!CTxdStore::ms_pTxdPool->IsFreeSlotAtIndex(id))
		{
			TxdDef* txd = CTxdStore::AddRef(id);
			return Txd{txd->m_pRwDictionary, id, "", false}; // name in this case is not recognizable
		}
		return sol::nullopt;
	}

	sol::optional<Txd> getTxdFromName(const std::string& name)
	{
		int id = CTxdStore::FindTxdSlot(name.c_str());
		if (id != -1)
			return getTxdFromId(id);
		return sol::nullopt;
	}

	Txd txdFromMemory(uintptr_t txd)
	{
		return Txd{reinterpret_cast<RwTexDictionary*>(txd), -1, "", false};
	}

	void initialize(sol::table& module)
	{
		module.set_function("load_txd", &loadTxdFromFile);
		module.set_function("get_txd", sol::overload(&getTxdFromName, &getTxdFromId));
		module.new_usertype<Txd>("txd", "new", sol::no_constructor,
								 "from_memory", sol::factories(&txdFromMemory),
								 "get_pointer", &Txd::getRawPointer,
								 "id", sol::property(&Txd::getId),
								 "name", sol::property(&Txd::getName), 
								 "get_texture", &Txd::getTexture);
	}
}

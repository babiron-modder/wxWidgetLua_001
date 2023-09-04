#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


#include <lua.hpp>
#include <map>
#include <vector>


class MyApp : public wxApp {
public:
	bool OnInit() { return true; }
};

namespace Lua {
	lua_State* state;
}



namespace wxcpp {
	namespace control {
		std::vector<void*> wx_components;

		int addComponent(wxWindow* pointer) {
			wx_components.push_back(pointer);
			int index_num = wx_components.size() - 1;
			return index_num;
		}

		wxWindow* getComponent(int id) {
			return (wxWindow*)wx_components[id];
		}
	}


	int Frame(lua_State* L) {
		wxFrame* frame = new wxFrame(NULL, wxID_ANY, wxString::FromUTF8(luaL_checkstring(L, 1)),
			wxPoint(luaL_checkinteger(L, 2), luaL_checkinteger(L, 3)),
			wxSize(luaL_checkinteger(L, 4), luaL_checkinteger(L, 5)));
		frame->SetBackgroundColour(wxColour(240, 240, 240));
		frame->Show(true);
		lua_pushinteger(L, wxcpp::control::addComponent(frame));
		return 1;
	}

	int StaticText(lua_State* L) {
		wxStaticText* statictext = new wxStaticText(
			wxcpp::control::getComponent(luaL_checkinteger(L, 1)),
			wxID_ANY, wxString::FromUTF8(luaL_checkstring(L, 2)),
			wxPoint(luaL_checkinteger(L, 3), luaL_checkinteger(L, 4)), wxSize(-1, -1));
		lua_pushinteger(L, wxcpp::control::addComponent(statictext));
		return 1;
	}

	int Button(lua_State* L) {
		wxButton *button = new wxButton(
			wxcpp::control::getComponent(luaL_checkinteger(L, 1)),
			wxID_ANY, wxString::FromUTF8(luaL_checkstring(L, 2)),
			wxPoint(luaL_checkinteger(L, 3), luaL_checkinteger(L, 4)),
			wxSize(luaL_checkinteger(L, 5), luaL_checkinteger(L, 6)),
			wxBORDER_SIMPLE);
		lua_pushinteger(L, wxcpp::control::addComponent(button));
		return 1;
	}

	// イベント関係
	std::map<wxWindowID, std::vector<std::string>> _eventlist;
	void _clickEvent(wxCommandEvent& e) {
		int id = ((wxWindow*)e.GetEventObject())->GetId();
		for (int i = 0; i < _eventlist[id].size(); ++i) {
			lua_getglobal(Lua::state, _eventlist[id][i].c_str());
			lua_pcall(Lua::state, 0, 0, 0);
		}
	}

	int addClickEvent(lua_State* L) {
		wxButton* button = (wxButton*)wxcpp::control::getComponent(luaL_checkinteger(L, 1));
		button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &_clickEvent);
		wxWindowID tmp_id = button->GetId();
		if (_eventlist.count(tmp_id) == 0) {
			_eventlist[tmp_id] = std::vector<std::string>();
		}
		_eventlist[tmp_id].push_back(luaL_checkstring(L, 2));
		return 1;
	}
}



int main(int argc, char* argv[]) {
	Lua::state = luaL_newstate();
	luaL_openlibs(Lua::state);


	// luaL_dostring(Lua::state, "print('hello lua script!!')");

	std::string filename = "";

	// 引数がない場合はファイル選択をさせる
	if (argc != 2) {
		wxFileDialog* file = new wxFileDialog(nullptr);
		if (file->ShowModal() == wxID_OK) {
			filename = file->GetPath();
		}
	}
	else
	{
		filename = std::string(argv[1]);
	}

	if (luaL_loadfile(Lua::state, filename.c_str()) == 0) {
		// ファイル読み込みが成功した場合のみ続ける
		lua_pcall(Lua::state, 0, 0, 0);

		// 関数登録
		lua_pushcfunction(Lua::state, wxcpp::Frame);
		lua_setglobal(Lua::state, "Frame");
		lua_pushcfunction(Lua::state, wxcpp::StaticText);
		lua_setglobal(Lua::state, "StaticText");
		lua_pushcfunction(Lua::state, wxcpp::Button);
		lua_setglobal(Lua::state, "Button");
		lua_pushcfunction(Lua::state, wxcpp::addClickEvent);
		lua_setglobal(Lua::state, "addClickEvent");


		// ウィンドウ準備
		wxApp::SetInstance(new MyApp());
		wxEntryStart(0, NULL);




		// Luaのmain呼び出し
		lua_getglobal(Lua::state, "main");
		lua_pcall(Lua::state, 0, 0, 0);









		wxTheApp->CallOnInit();
		wxTheApp->OnRun();
		wxTheApp->OnExit();
		wxEntryCleanup();
	}



	lua_close(Lua::state);




	return 0;
}
#pragma once

// Language of the port's own UI (the launcher window and the settings overlay).
// This is independent from the game's language (language.h): that one selects
// the text of the emulated European disc, this one selects the text of the
// host windows.
enum class UiLanguage {
    Spanish = 0,
    English = 1,
};

UiLanguage GetUiLanguage();
void SetUiLanguage(UiLanguage language);

// Fixed labels for the two selector entries.
const wchar_t* UiLanguageLabel(UiLanguage language);

// Returns the English rendering of a Spanish UI string when the UI language is
// English, or `spanish` itself otherwise. Strings with no English entry fall
// back to Spanish, so a half-filled table keeps working.
const wchar_t* UiTranslate(const wchar_t* spanish);

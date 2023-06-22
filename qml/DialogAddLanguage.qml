import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    property alias languageName: textField.text
    title: qsTr("Add Language")
    standardButtons: Dialog.Ok | Dialog.Cancel
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: parent.width
    height: parent.height
    clip: true
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        RowLayout {
            spacing: 10
            Label {
                text: qsTr("Language name")
            }
            CustomComboBox {
                id: comboBoxLanguages
                model: ListModel {
                    id: listModelLanguages
                    ListElement { text: "Afar"; value: "aa" }
                    ListElement { text: "Abkhazian"; value: "ab" }
                    ListElement { text: "Avestan"; value: "ae" }
                    ListElement { text: "Afrikaans"; value: "af" }
                    ListElement { text: "Akan"; value: "ak" }
                    ListElement { text: "Amharic"; value: "am" }
                    ListElement { text: "Aragonese"; value: "an" }
                    ListElement { text: "Arabic"; value: "ar" }
                    ListElement { text: "Assamese"; value: "as" }
                    ListElement { text: "Avaric"; value: "av" }
                    ListElement { text: "Aymara"; value: "ay" }
                    ListElement { text: "Azerbaijani"; value: "az" }
                    ListElement { text: "Bashkir"; value: "ba" }
                    ListElement { text: "Belarusian"; value: "be" }
                    ListElement { text: "Bulgarian"; value: "bg" }
                    ListElement { text: "Bihari languages"; value: "bh" }
                    ListElement { text: "Bislama"; value: "bi" }
                    ListElement { text: "Bambara"; value: "bm" }
                    ListElement { text: "Bengali"; value: "bn" }
                    ListElement { text: "Tibetan"; value: "bo" }
                    ListElement { text: "Breton"; value: "br" }
                    ListElement { text: "Bosnian"; value: "bs" }
                    ListElement { text: "Catalan"; value: "ca" }
                    ListElement { text: "Chechen"; value: "ce" }
                    ListElement { text: "Chamorro"; value: "ch" }
                    ListElement { text: "Corsican"; value: "co" }
                    ListElement { text: "Cree"; value: "cr" }
                    ListElement { text: "Czech"; value: "cs" }
                    ListElement { text: "Church Slavic"; value: "cu" }
                    ListElement { text: "Chuvash"; value: "cv" }
                    ListElement { text: "Welsh"; value: "cy" }
                    ListElement { text: "Danish"; value: "da" }
                    ListElement { text: "German"; value: "de" }
                    ListElement { text: "Dhivehi"; value: "dv" }
                    ListElement { text: "Dzongkha"; value: "dz" }
                    ListElement { text: "Ewe"; value: "ee" }
                    ListElement { text: "Modern Greek (1453-)"; value: "el" }
                    ListElement { text: "English"; value: "en" }
                    ListElement { text: "Esperanto"; value: "eo" }
                    ListElement { text: "Spanish"; value: "es" }
                    ListElement { text: "Estonian"; value: "et" }
                    ListElement { text: "Basque"; value: "eu" }
                    ListElement { text: "Persian"; value: "fa" }
                    ListElement { text: "Fulah"; value: "ff" }
                    ListElement { text: "Finnish"; value: "fi" }
                    ListElement { text: "Fijian"; value: "fj" }
                    ListElement { text: "Faroese"; value: "fo" }
                    ListElement { text: "French"; value: "fr" }
                    ListElement { text: "Western Frisian"; value: "fy" }
                    ListElement { text: "Irish"; value: "ga" }
                    ListElement { text: "Scottish Gaelic"; value: "gd" }
                    ListElement { text: "Galician"; value: "gl" }
                    ListElement { text: "Guarani"; value: "gn" }
                    ListElement { text: "Gujarati"; value: "gu" }
                    ListElement { text: "Manx"; value: "gv" }
                    ListElement { text: "Hausa"; value: "ha" }
                    ListElement { text: "Hebrew"; value: "he" }
                    ListElement { text: "Hindi"; value: "hi" }
                    ListElement { text: "Hiri Motu"; value: "ho" }
                    ListElement { text: "Croatian"; value: "hr" }
                    ListElement { text: "Haitian"; value: "ht" }
                    ListElement { text: "Hungarian"; value: "hu" }
                    ListElement { text: "Armenian"; value: "hy" }
                    ListElement { text: "Herero"; value: "hz" }
                    ListElement { text: "Interlingua (International Auxiliary Language"; value: "ia" }
                    ListElement { text: "Indonesian"; value: "id" }
                    ListElement { text: "Interlingue"; value: "ie" }
                    ListElement { text: "Igbo"; value: "ig" }
                    ListElement { text: "Sichuan Yi"; value: "ii" }
                    ListElement { text: "Inupiaq"; value: "ik" }
                    ListElement { text: "Indonesian"; value: "in" }
                    ListElement { text: "Ido"; value: "io" }
                    ListElement { text: "Icelandic"; value: "is" }
                    ListElement { text: "Italian"; value: "it" }
                    ListElement { text: "Inuktitut"; value: "iu" }
                    ListElement { text: "Hebrew"; value: "iw" }
                    ListElement { text: "Japanese"; value: "ja" }
                    ListElement { text: "Yiddish"; value: "ji" }
                    ListElement { text: "Javanese"; value: "jv" }
                    ListElement { text: "Javanese"; value: "jw" }
                    ListElement { text: "Georgian"; value: "ka" }
                    ListElement { text: "Kongo"; value: "kg" }
                    ListElement { text: "Kikuyu"; value: "ki" }
                    ListElement { text: "Kuanyama"; value: "kj" }
                    ListElement { text: "Kazakh"; value: "kk" }
                    ListElement { text: "Kalaallisut"; value: "kl" }
                    ListElement { text: "Khmer"; value: "km" }
                    ListElement { text: "Kannada"; value: "kn" }
                    ListElement { text: "Korean"; value: "ko" }
                    ListElement { text: "Kanuri"; value: "kr" }
                    ListElement { text: "Kashmiri"; value: "ks" }
                    ListElement { text: "Kurdish"; value: "ku" }
                    ListElement { text: "Komi"; value: "kv" }
                    ListElement { text: "Cornish"; value: "kw" }
                    ListElement { text: "Kirghiz"; value: "ky" }
                    ListElement { text: "Latin"; value: "la" }
                    ListElement { text: "Luxembourgish"; value: "lb" }
                    ListElement { text: "Ganda"; value: "lg" }
                    ListElement { text: "Limburgan"; value: "li" }
                    ListElement { text: "Lingala"; value: "ln" }
                    ListElement { text: "Lao"; value: "lo" }
                    ListElement { text: "Lithuanian"; value: "lt" }
                    ListElement { text: "Luba-Katanga"; value: "lu" }
                    ListElement { text: "Latvian"; value: "lv" }
                    ListElement { text: "Malagasy"; value: "mg" }
                    ListElement { text: "Marshallese"; value: "mh" }
                    ListElement { text: "Maori"; value: "mi" }
                    ListElement { text: "Macedonian"; value: "mk" }
                    ListElement { text: "Malayalam"; value: "ml" }
                    ListElement { text: "Mongolian"; value: "mn" }
                    ListElement { text: "Moldavian"; value: "mo" }
                    ListElement { text: "Marathi"; value: "mr" }
                    ListElement { text: "Malay (macrolanguage)"; value: "ms" }
                    ListElement { text: "Maltese"; value: "mt" }
                    ListElement { text: "Burmese"; value: "my" }
                    ListElement { text: "Nauru"; value: "na" }
                    ListElement { text: "Norwegian Bokmål"; value: "nb" }
                    ListElement { text: "North Ndebele"; value: "nd" }
                    ListElement { text: "Nepali (macrolanguage)"; value: "ne" }
                    ListElement { text: "Ndonga"; value: "ng" }
                    ListElement { text: "Dutch"; value: "nl" }
                    ListElement { text: "Norwegian Nynorsk"; value: "nn" }
                    ListElement { text: "Norwegian"; value: "no" }
                    ListElement { text: "South Ndebele"; value: "nr" }
                    ListElement { text: "Navajo"; value: "nv" }
                    ListElement { text: "Nyanja"; value: "ny" }
                    ListElement { text: "Occitan (post 1500)"; value: "oc" }
                    ListElement { text: "Ojibwa"; value: "oj" }
                    ListElement { text: "Oromo"; value: "om" }
                    ListElement { text: "Oriya (macrolanguage)"; value: "or" }
                    ListElement { text: "Ossetian"; value: "os" }
                    ListElement { text: "Panjabi"; value: "pa" }
                    ListElement { text: "Pali"; value: "pi" }
                    ListElement { text: "Polish"; value: "pl" }
                    ListElement { text: "Pushto"; value: "ps" }
                    ListElement { text: "Portuguese"; value: "pt" }
                    ListElement { text: "Quechua"; value: "qu" }
                    ListElement { text: "Romansh"; value: "rm" }
                    ListElement { text: "Rundi"; value: "rn" }
                    ListElement { text: "Romanian"; value: "ro" }
                    ListElement { text: "Russian"; value: "ru" }
                    ListElement { text: "Kinyarwanda"; value: "rw" }
                    ListElement { text: "Sanskrit"; value: "sa" }
                    ListElement { text: "Sardinian"; value: "sc" }
                    ListElement { text: "Sindhi"; value: "sd" }
                    ListElement { text: "Northern Sami"; value: "se" }
                    ListElement { text: "Sango"; value: "sg" }
                    ListElement { text: "Serbo-Croatian"; value: "sh" }
                    ListElement { text: "Sinhala"; value: "si" }
                    ListElement { text: "Slovak"; value: "sk" }
                    ListElement { text: "Slovenian"; value: "sl" }
                    ListElement { text: "Samoan"; value: "sm" }
                    ListElement { text: "Shona"; value: "sn" }
                    ListElement { text: "Somali"; value: "so" }
                    ListElement { text: "Albanian"; value: "sq" }
                    ListElement { text: "Serbian"; value: "sr" }
                    ListElement { text: "Swati"; value: "ss" }
                    ListElement { text: "Southern Sotho"; value: "st" }
                    ListElement { text: "Sundanese"; value: "su" }
                    ListElement { text: "Swedish"; value: "sv" }
                    ListElement { text: "Swahili (macrolanguage)"; value: "sw" }
                    ListElement { text: "Tamil"; value: "ta" }
                    ListElement { text: "Telugu"; value: "te" }
                    ListElement { text: "Tajik"; value: "tg" }
                    ListElement { text: "Thai"; value: "th" }
                    ListElement { text: "Tigrinya"; value: "ti" }
                    ListElement { text: "Turkmen"; value: "tk" }
                    ListElement { text: "Tagalog"; value: "tl" }
                    ListElement { text: "Tswana"; value: "tn" }
                    ListElement { text: "Tonga (Tonga Islands)"; value: "to" }
                    ListElement { text: "Turkish"; value: "tr" }
                    ListElement { text: "Tsonga"; value: "ts" }
                    ListElement { text: "Tatar"; value: "tt" }
                    ListElement { text: "Twi"; value: "tw" }
                    ListElement { text: "Tahitian"; value: "ty" }
                    ListElement { text: "Uighur"; value: "ug" }
                    ListElement { text: "Ukrainian"; value: "uk" }
                    ListElement { text: "Urdu"; value: "ur" }
                    ListElement { text: "Uzbek"; value: "uz" }
                    ListElement { text: "Venda"; value: "ve" }
                    ListElement { text: "Vietnamese"; value: "vi" }
                    ListElement { text: "Volapük"; value: "vo" }
                    ListElement { text: "Walloon"; value: "wa" }
                    ListElement { text: "Wolof"; value: "wo" }
                    ListElement { text: "Xhosa"; value: "xh" }
                    ListElement { text: "Yiddish"; value: "yi" }
                    ListElement { text: "Yoruba"; value: "yo" }
                    ListElement { text: "Zhuang"; value: "za" }
                    ListElement { text: "Chinese"; value: "zh" }
                    ListElement { text: "Zulu"; value: "zu" }
                }
                textRole: "text"
                delegate: ItemDelegate {
                    text: model.text
                }
                onCurrentIndexChanged: {
                    var language = listModelLanguages.get(currentIndex);

                    textField.text = language.value;
                }
            }
            CustomTextField {
                id: textField
                leftPadding: 10
                rightPadding: 10
                topPadding: 10
                bottomPadding: 10
                placeholderText: "de|en|es|fr..."
                color: labelError.visible ? "red" : ""
                onTextChanged: {
                    for (var i = 0; i < listModelLanguages.count; i++) {
                        var language = listModelLanguages.get(i);

                        if (language.value === textField.text) {
                            comboBoxLanguages.currentIndex = i;
                        }
                    }

                    lexiconProcessor.isAlreadyUsedLanguageName(text);
                }
                Connections {
                    target: lexiconProcessor
                    ignoreUnknownSignals: true
                    function onIsAlreadyUsedLanguageNameResult(data) {
                        data = JSON.parse(data);

                        labelError.visible = data.isAlreadyUsedLanguageName
                    }
                }
            }
        }
        Label {
            id: labelError
            color: "red"
            text: qsTr("language is already used")
            visible: false
        }
        Item {
            Layout.fillHeight: true
        }
    }
}

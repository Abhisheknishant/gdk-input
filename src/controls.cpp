// © 2019 Joseph Cameron - All Rights Reserved

#include <type_traits>
#include <iostream>

#include <jsoncons/json.hpp>
#include <magic_enum/magic_enum.hpp>

#include <gdk/controls.h>

/*#include <nlohmann/json.hpp>
using namespace nlohmann;

//TODO: move to separate library. To be used as a submodule containing a nlohmann json release + util header
namespace jfc::nlohmann::json_util
{
    using namespace nlohmann;

    std::optional<json> get_property_optional(const json &object, const std::string &propertyName, const json::value_t &type)
    {
        return object.contains(propertyName) && object[propertyName].type() == type
            ? std::optional<json>(object[propertyName])
            : std::nullopt;
    }
    
    json get_property_required(const json &object, const std::string &propertyName, const json::value_t &type, const std::string &errorMessage = "")
    {
        if (object.contains(propertyName) && object[propertyName].type() == type) return object[propertyName];

        throw std::invalid_argument(errorMessage == "" 
            ? "required JSON property \"" + propertyName + "\" does not exist"
            : errorMessage);
    }
}
using namespace jfc::nlohmann;
*/

/// \brief data model for use storing/transmitting controls instance state
/*struct controls_serial_model
{
    using key_collection_type = std::set<gdk::keyboard::Key>;
    using mouse_button_collection_type = std::set<std::string>;

    
    ////////////////////
    std::set<gdk::keyboard::Key> coolkeys;
    ////////////////////

    struct binding_type
    {
        key_collection_type keys;

        struct
        {
            mouse_button_collection_type buttons;

            //mouse_axis_collection_type axes;
        } mouse;

        //std::map<std::string, gamepad_bindings> gamepads; //!< bindings for supported gamepads
    };
    
    std::map<std::string, binding_type> bindings;
};*/

//TODO: move to separate repo. jscons_wrapper? jsoncons+util? jsoncons_package? Yes. Expand magic enum requirements.
#define JSONCONS_UTIL_ENUM_TRAITS_DECL_BASE(CharT, EnumType)  \
namespace jsoncons \
{ \
    template<typename Json> \
    struct json_type_traits<Json, EnumType,typename std::enable_if<std::is_same<typename Json::char_type,CharT>::value>::type> \
    { \
        static_assert(std::is_enum<EnumType>::value, # EnumType " must be an enum"); \
        typedef EnumType value_type; \
        typedef std::basic_string<CharT> string_type; \
        typedef basic_string_view<CharT> string_view_type; \
        typedef typename Json::allocator_type allocator_type; \
        typedef std::pair<EnumType,string_type> mapped_type; \
 \
        static std::pair<const mapped_type*,const mapped_type*> get_values()/*TODO: think about static array rather than vec. Think about expanding magic_enum implementatiosn, to remove dependency on that porject*/ \
        { \
            static const std::vector<mapped_type> v = []() \
            { \
                std::remove_const<decltype(v)>::type buffer; \
             \
                auto values = magic_enum::enum_values<EnumType>(); \
 \
                buffer.reserve(magic_enum::enum_count<EnumType>()); \
 \
                for (const auto &value : values) \
                { \
                    buffer.push_back({value, string_type(magic_enum::enum_name<EnumType>(value))}); \
                } \
 \
                return buffer; \
            }(); \
 \
            return std::make_pair(&(*v.begin()), &(*v.end())); \
        } \
 \
        static bool is(const Json& ajson) noexcept \
        { \
            if (!ajson.is_string()) return false; \
 \
            auto first = get_values().first; \
            auto last = get_values().second; \
 \
            const string_view_type s = ajson.template as<string_view_type>(); \
 \
            if (s.empty() && std::find_if(first, last, [](const mapped_type& item) -> bool \
                {  \
                    return item.first == value_type();  \
                }) == last) \
            { \
                return true; \
            } \
 \
            auto it = std::find_if(first, last, [&](const mapped_type& item) -> bool \
            {  \
                return item.second == s;  \
            }); \
 \
            return it != last; \
        } \
        static value_type as(const Json& ajson) \
        { \
            if (!ajson.is_string()) \
            { \
                JSONCONS_THROW(json_runtime_error<std::runtime_error>("Not an enum")); \
            } \
 \
            const string_view_type s = ajson.template as<string_view_type>(); \
 \
            auto first = get_values().first; \
 \
            auto last = get_values().second; \
 \
            if (s.empty() && std::find_if(first, last, [](const mapped_type& item) -> bool \
                {  \
                    return item.first == value_type();  \
                }) == last) \
            { \
                return value_type(); \
            } \
 \
            auto it = std::find_if(first, last, [&](const mapped_type& item) -> bool \
            {  \
                return item.second == s;  \
            }); \
 \
            if (it == last) \
            { \
                if (s.empty()) \
                { \
                    return value_type(); \
                } \
                else \
                { \
                    JSONCONS_THROW(json_runtime_error<std::runtime_error>("Not an enum")); \
                } \
            } \
            return it->first; \
        } \
        static Json to_json(value_type aval, allocator_type allocator=allocator_type()) \
        { \
            static constexpr CharT empty_string[] = {0}; \
 \
            auto first = get_values().first; \
 \
            auto last = get_values().second; \
 \
            auto it = std::find_if(first, last, [aval](const mapped_type& item) -> bool \
            {  \
                return item.first == aval;  \
            }); \
 \
            if (it == last) \
            { \
                if (aval == value_type()) \
                { \
                    return Json(empty_string); \
                } \
                else \
                { \
                    JSONCONS_THROW(json_runtime_error<std::runtime_error>("Not an enum")); \
                } \
            } \
 \
            return Json(it->second,allocator); \
        } \
    }; \
} \

#define JSONCONS_UTIL_ENUM_TRAITS_DECL(EnumType)  \
    JSONCONS_UTIL_ENUM_TRAITS_DECL_BASE(char,EnumType) \
    //JSONCONS_UTIL_ENUM_TRAITS_DECL_BASE(wchar_t, EnumType) \ //magic_enum does nto output to wchar_t. Would have to modify to support type param for string_view

namespace gdk
{
    /// \brief implementation of controls. hidden to break depedencies. went with factory + interface instead of pImpl since 
    /// both solutions would introduce deref costs but factory + interface leaves room for alternative implementations etc. 
    /// + is a pattern recognized outside of C++
    class controls_impl : public controls
    {
    public:
        using key_collection_type = std::set<keyboard::Key>;

        using mouse_button_collection_type = std::set<mouse::Button>;
        using mouse_axis_collection_type = std::set<std::pair<mouse::Axis, /*scale and direction*/double>>;
        
        using gamepad_button_collection_type = std::set</*index*/int>;
        using gamepad_axis_collection_type = std::map</*index*/int, /*deadzone*/float>;
        using gamepad_hat_collection_type = std::map</*index*/int, /*hat direction*/gamepad::hat_state_type>;

        struct gamepad_bindings
        {
            gamepad_button_collection_type buttons;
            
            gamepad_axis_collection_type axes;

            gamepad_hat_collection_type hats;
        };

        struct bindings
        {
            key_collection_type keys;

            struct
            {
                mouse_button_collection_type buttons;

                mouse_axis_collection_type axes;
            } mouse;

            std::map<std::string, gamepad_bindings> gamepads; //!< bindings for supported gamepads
        };

        std::map<std::string, bindings> m_Inputs; //TODO: rename to input map?

        std::shared_ptr<keyboard> m_pKeyboard;
        std::shared_ptr<mouse>    m_pMouse;
        std::shared_ptr<gamepad>  m_pGamepad;

    public:
        virtual double get(const std::string &aName) const override;

        void setKeyboard(std::shared_ptr<keyboard> aKeyboard) override;

        void setMouse(std::shared_ptr<mouse> aMouse) override;
        
        void setGamepad(std::shared_ptr<gamepad> aGamepad) override;

        void addKeyToMapping(const std::string &aMappingName, const keyboard::Key aKey) override;

        void addMouseButtonToMapping(const std::string &aMappingName, const mouse::Button aButton) override;
        
        void addMouseAxisToMapping(const std::string &aMappingName, const mouse::Axis aAxis, const double scaleAndDirection) override;

        void addGamepadButtonToMapping(const std::string &aMappingName, const std::string &aGamepadName, const int aButtonIndex) override;
        
        void addGamepadAxisToMapping(const std::string &aMappingName, const std::string &aGamepadName, const int aAxisIndex, const float aMinimum) override;
       
        void addGamepadHatToMapping(const std::string &aMappingName, const std::string &aGamepadName, const int aHatIndex, const gamepad::hat_state_type aHatState) override;

        //! adds bindings from a string containing JSON data
        //void addMappingsFromJSON(const std::string &aJSONData) override;

        //virtual void deserializeFromJSON(const std::string &json) override;
        
        virtual std::string serializeToJSON() override;
        
        controls_impl(std::shared_ptr<keyboard> aKeyboard = nullptr, 
            std::shared_ptr<mouse> aMouse = nullptr, 
            std::shared_ptr<gamepad> aGamepad = nullptr);

        controls_impl(const std::string &json,
                std::shared_ptr<keyboard> aKeyboard, 
                std::shared_ptr<mouse> aMouse, 
                std::shared_ptr<gamepad> aGamepad);

        ~controls_impl() = default;
    };
    
    std::unique_ptr<controls> controls::make(std::shared_ptr<keyboard> aKeyboard, 
        std::shared_ptr<mouse> aMouse, 
        std::shared_ptr<gamepad> aGamepad)
    {
        return std::make_unique<controls_impl>(controls_impl(aKeyboard, aMouse, aGamepad));
    }

    std::unique_ptr<controls> controls::make_from_json(const std::string &json,
        std::shared_ptr<keyboard> aKeyboard, 
        std::shared_ptr<mouse> aMouse, 
        std::shared_ptr<gamepad> aGamepad)
    {
        auto instance = jsoncons::decode_json<gdk::controls_impl>(json);

        //std::cout << 

        return std::make_unique<controls_impl>(controls_impl(json, aKeyboard, aMouse, aGamepad));
    }


    controls_impl::controls_impl(std::shared_ptr<keyboard> aKeyboard, 
        std::shared_ptr<mouse> aMouse, 
        std::shared_ptr<gamepad> aGamepad)    
    : m_pKeyboard(aKeyboard)
    , m_pMouse(aMouse)
    , m_pGamepad(aGamepad)
    {}

    controls_impl::controls_impl(const std::string &json,
        std::shared_ptr<keyboard> aKeyboard, 
        std::shared_ptr<mouse> aMouse, 
        std::shared_ptr<gamepad> aGamepad)    
    : m_pKeyboard(aKeyboard)
    , m_pMouse(aMouse)
    , m_pGamepad(aGamepad)
    {
        ///...... todo.....
    }

    double controls_impl::get(const std::string &aName) const
    {
        auto iter = m_Inputs.find(aName); 

        if (iter == m_Inputs.end()) throw std::invalid_argument(std::string("TAG").append("not a valid button: ").append(aName));

        if (m_pKeyboard)
        {
            for (const auto &key : iter->second.keys)
            {
                if (const auto value = static_cast<float>(m_pKeyboard->getKeyDown(key))) return value;
            }
        }

        if (m_pMouse) 
        {
            for (const auto &button : iter->second.mouse.buttons)
            {
                if (const auto value = static_cast<float>(m_pMouse->getButtonDown(button))) return value;
            }

            //TODO: Mouse axes
            for (const auto &axis : iter->second.mouse.axes)
            {
                if (m_pMouse->getCursorMode() == gdk::mouse::CursorMode::Locked)
                {
                    const auto delta = m_pMouse->getDelta();

                    switch(axis.first)
                    {
                        case mouse::Axis::X:
                        {
                            const auto value = delta.x;

                            if (axis.second > 0 && delta.x > 0) return value * axis.second; 

                            if (axis.second < 0 && delta.x < 0) return value * axis.second; 
                        } break;

                        case mouse::Axis::Y:
                        {
                            const auto value = delta.y;

                            if (axis.second > 0 && delta.y > 0) return value * axis.second; 

                            if (axis.second < 0 && delta.y < 0) return value * axis.second; 
                        } break;

                        default: throw std::invalid_argument("unhandled axis type");
                    }
                }
            }
        }
       
        if (m_pGamepad)
        {
            if (const auto current_gamepad_iter = iter->second.gamepads.find(std::string(m_pGamepad->getName())); current_gamepad_iter != iter->second.gamepads.end())
            {
                for (const auto &button : current_gamepad_iter->second.buttons)
                {   
                    if (const auto value = static_cast<float>(m_pGamepad->getButtonDown(button))) return value;
                }

                for (const auto &hat : current_gamepad_iter->second.hats)
                {   
                    if (auto a = hat.second, b = m_pGamepad->getHat(hat.first); a.x == b.x && a.y == b.y) return 1;
                }

                for (const auto &axis : current_gamepad_iter->second.axes)
                {
                    if (const auto value = static_cast<float>(m_pGamepad->getAxis(axis.first))) 
                    {
                        ////
                        const float minimum = axis.second;

                        if (minimum >= 0 && value > minimum) return value; 
                        else if (minimum < 0 && value < minimum) return value * -1;
                    }
                }
            }
            else std::cerr << m_pGamepad->getName() << " not configured for these controls_impl\n";
        }

        return 0;
    }

    void controls_impl::setKeyboard(std::shared_ptr<keyboard> aKeyboard)
    {
        m_pKeyboard = aKeyboard;
    }

    void controls_impl::setMouse(std::shared_ptr<mouse> aMouse)
    {
        m_pMouse = aMouse;
    }

    void controls_impl::setGamepad(std::shared_ptr<gamepad> aGamepad)
    {
        m_pGamepad = aGamepad;
    }
    
    void controls_impl::addKeyToMapping(const std::string &aName, const keyboard::Key aKey)
    {
        m_Inputs[aName].keys.insert(aKey);
    }
    
    void controls_impl::addMouseButtonToMapping(const std::string &aName, const mouse::Button aButton)
    {
        m_Inputs[aName].mouse.buttons.insert(aButton);
    }

    void controls_impl::addMouseAxisToMapping(const std::string &aName, const mouse::Axis aAxis, const double aScaleAndDirection)
    {
        m_Inputs[aName].mouse.axes.insert({aAxis, aScaleAndDirection});
    }

    void controls_impl::addGamepadAxisToMapping(const std::string &aInputName, const std::string &aGamepadName, const int aAxisIndex, const float aMinimum)
    {
        m_Inputs[aInputName].gamepads[aGamepadName].axes.insert({aAxisIndex, aMinimum});
    }

    void controls_impl::addGamepadHatToMapping(const std::string &aInputName, const std::string &aGamepadName, const int aHatIndex, const gamepad::hat_state_type aHatState)
    {
        m_Inputs[aInputName].gamepads[aGamepadName].hats.insert({aHatIndex, aHatState});
    }

    void controls_impl::addGamepadButtonToMapping(const std::string &aInputName, const std::string &aGamepadName, const int aButtonIndex)
    {
        m_Inputs[aInputName].gamepads[aGamepadName].buttons.insert(aButtonIndex);
    }

    std::string controls_impl::serializeToJSON()
    {
        std::string s;
        jsoncons::encode_json(*this, s, jsoncons::indenting::indent);

        return s;
    }
}

JSONCONS_UTIL_ENUM_TRAITS_DECL(gdk::keyboard::Key);

JSONCONS_UTIL_ENUM_TRAITS_DECL(gdk::mouse::Button);
JSONCONS_UTIL_ENUM_TRAITS_DECL(gdk::mouse::Axis);

JSONCONS_UTIL_ENUM_TRAITS_DECL(gdk::gamepad::hat_state_type::vertical_direction);
JSONCONS_UTIL_ENUM_TRAITS_DECL(gdk::gamepad::hat_state_type::horizontal_direction);

JSONCONS_MEMBER_TRAITS_DECL(gdk::gamepad::hat_state_type, x, y);

JSONCONS_MEMBER_TRAITS_DECL(gdk::controls_impl::gamepad_bindings, buttons/*, axes, hats*/);
JSONCONS_MEMBER_TRAITS_DECL(decltype(gdk::controls_impl::bindings::mouse), buttons);
JSONCONS_MEMBER_TRAITS_DECL(gdk::controls_impl::bindings, keys, gamepads, mouse);

JSONCONS_MEMBER_TRAITS_DECL(gdk::controls_impl, m_Inputs);


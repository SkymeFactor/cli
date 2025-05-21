#pragma once
#include <cli/cli.h>

namespace cli {

class AliasedMenu : public Menu
    {
    public:
        using strvec_t = std::vector<std::string>;

        // disable value and move semantics
        AliasedMenu(const AliasedMenu&) = delete;
        AliasedMenu& operator = (const AliasedMenu&) = delete;
        AliasedMenu(AliasedMenu&&) = delete;
        AliasedMenu& operator = (AliasedMenu&&) = delete;

        AliasedMenu() : Menu() {}

        explicit AliasedMenu(const std::string& _name, std::string desc = "(menu)") :
            Menu(_name, desc, "")
        {}

        // template <class F>
        void add_aliases_fetcher(std::function<strvec_t()> fetcher)
        {
            fetch_aliases_ = fetcher;
        }

        bool Exec(const std::vector<std::string>& cmdLine, CliSession& session) override
        {
            bool ret = HandleCommand({fetch_aliases_()}, cmdLine, session);
            if (ret && cmdLine.size() == 1) {
                prompt_ = cmdLine[0];
            }
            return ret;
        }

        bool ExecParent(const std::vector<std::string>& cmdLine, CliSession& session) override
        {
            auto aliases = fetch_aliases_();
            aliases.emplace_back(ParentShortcut());

            bool ret = HandleCommand(aliases, cmdLine, session);
            if (ret && cmdLine.size() == 1) {
                prompt_ = cmdLine[0];
            }
            return ret;
        }

        std::string Prompt() const override
        {
            return prompt_;
        }

        void Help(std::ostream& out) const override
        {
            if (!IsEnabled()) return;
            out << " - " << Name();
            auto aliases = fetch_aliases_();
            if (!aliases.empty()) {
                out << " {";
                if (aliases.size() > 1) {
                    out << aliases.front();
                }
                std::for_each(aliases.begin() + 1, aliases.end(), [&](const std::string& alias){
                    out << '|' << alias;
                });
                out << '}';
            }
            out << "\n\t" << Description() << "\n";
        }

        /**
         * Retrieves completion suggestions for the user input recursively.
         *
         * This function checks if the user input starts with the current command's name. If it
         * does, it extracts the remaining part of the input and retrieves suggestions:
         *   - From subcommands using their `GetCompletionRecursive` function.
         *   - From the parent command (if available) using its `GetCompletionRecursiveFull` function.
         *   - (Optional) You can customize the behavior for empty lines to provide top-level commands.
         *
         * If the input doesn't start with the command name, it delegates to the base class's
         * `Command::GetCompletionRecursive` function for handling generic commands.
         *
         * @param line The user's input string (potentially incomplete command).
         * @return A vector containing suggested completions for the user input.
         */
        std::vector<std::string> GetCompletionRecursive(const std::string& line) const override
        {
            for (const auto& name : fetch_aliases_()) {
                if (line.rfind(name, 0) == 0) // line starts_with Name()
                {
                    return Menu::GetCompletionRecursiveHelper(line, {name});
                }
            }

            return Command::GetCompletionRecursive(line);
        }

    private:

        std::function<strvec_t()> fetch_aliases_ {};
        std::string prompt_;
    };


    std::unique_ptr<Menu>
    make_volatile_menu(const std::string& _name, std::string desc = "(dynamic menu)", std::function<std::vector<std::string>()> aliases_fetcher_ = {})
    {
        auto vol_menu = std::make_unique<cli::AliasedMenu>(_name, desc);
        vol_menu->add_aliases_fetcher(aliases_fetcher_);
        return std::unique_ptr<Menu>{static_cast<cli::Menu*>(vol_menu.release())};
    }

}
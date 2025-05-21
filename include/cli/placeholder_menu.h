#ifndef CLI_PLACEHOLDER_H
#define CLI_PLACEHOLDER_H

#include <cli/cli.h>

namespace cli {

class PlaceholderMenu : public Menu
    {
    public:
        using strvec_t = std::vector<std::string>;

        // disable value and move semantics
        PlaceholderMenu(const PlaceholderMenu&) = delete;
        PlaceholderMenu& operator = (const PlaceholderMenu&) = delete;
        PlaceholderMenu(PlaceholderMenu&&) = delete;
        PlaceholderMenu& operator = (PlaceholderMenu&&) = delete;

        PlaceholderMenu() : Menu() {}

        explicit PlaceholderMenu(const std::string& _name, std::string desc = "(menu)") :
            Menu(_name, desc, "")
        {}

        // template <class F>
        void add_aliases_fetcher(std::function<strvec_t()> fetcher)
        {
            fetch_aliases_ = fetcher;
        }

        void add_alias_notifier(std::function<void(std::string)> notifier)
        {
            notify_on_select_ = notifier;
        }

        bool Exec(const std::vector<std::string>& cmdLine, CliSession& session) override
        {
            prompt_ = cmdLine[0];
            notify_on_select_(prompt_);
            return HandleCommand({fetch_aliases_()}, cmdLine, session);
        }

        bool ExecParent(const std::vector<std::string>& cmdLine, CliSession& session) override
        {
            auto aliases = fetch_aliases_();
            aliases.emplace_back(ParentShortcut());

            return HandleCommand(aliases, cmdLine, session);
        }

        std::string Prompt() const override
        {
            return prompt_;
        }

        void Help(std::ostream& out) const override
        {
            if (!IsEnabled()) return;
            
            for (const auto& alias : fetch_aliases_()) {
                out << " - " << alias << "\n\t" << Description() << "\n";
            }
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
                if (name.rfind(line, 0) == 0) return {name};
            }

            return Command::GetCompletionRecursive(line);
        }

    private:

        std::function<strvec_t()> fetch_aliases_ {};
        std::function<void(std::string)> notify_on_select_ {};
        std::string prompt_;
    };


    std::unique_ptr<Menu>
    make_placeholder_menu(const std::string& _name,
        std::function<std::vector<std::string>()> alias_fetcher_ = {},
        std::function<void(std::string)> on_select_notifier_ = {},
        std::string desc = "(dynamic menu)"
    )
    {
        auto vol_menu = std::make_unique<cli::PlaceholderMenu>(_name, desc);
        vol_menu->add_aliases_fetcher(alias_fetcher_);
        vol_menu->add_alias_notifier(on_select_notifier_);
        return std::unique_ptr<Menu>{static_cast<cli::Menu*>(vol_menu.release())};
    }

} // namespace cli

#endif // CLI_PLACEHOLDER_H

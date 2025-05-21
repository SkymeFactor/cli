#include <cli/placeholder_menu.h>
#include <cli/clifilesession.h>

int main(int argc, char* argv[]) {
    // setup cli
    std::string placeholder_value;

    auto rootMenu = std::make_unique< cli::Menu >( "cli" );
    auto menu_placeholder = cli::make_placeholder_menu("test",
        [&]()->std::vector<std::string>{ return {"foo", "bar", "baz"}; },
        [&](std::string alias) { placeholder_value = std::move(alias); }
    );
    menu_placeholder -> Insert(
            "hello",
            [&](std::ostream& out){ out << "Hello, from " << placeholder_value << "\n"; },
            "Print hello world" );
    menu_placeholder -> Insert(
            "hello_everysession",
            [&](std::ostream&){ cli::Cli::cout() << "Hello, everybody, it's " << placeholder_value << std::endl; },
            "Print hello everybody on all open sessions" );
    rootMenu -> Insert(
            "answer",
            [](std::ostream& out, int x){ out << "The answer is: " << x << "\n"; },
            "Print the answer to Life, the Universe and Everything " );
    rootMenu -> Insert(
            "color",
            [](std::ostream& out){ out << "Colors ON\n"; cli::SetColor(); },
            "Enable colors in the cli" );
    rootMenu -> Insert(
            "nocolor",
            [](std::ostream& out){ out << "Colors OFF\n"; cli::SetNoColor(); },
            "Disable colors in the cli" );

    auto subMenu = std::make_unique< cli::Menu >( "sub" );
    subMenu -> Insert(
            "hello",
            [](std::ostream& out){ out << "Hello, submenu world\n"; },
            "Print hello world in the submenu" );
    subMenu -> Insert(
            "demo",
            [](std::ostream& out){ out << "This is a sample!\n"; },
            "Print a demo string" );

    auto subSubMenu = std::make_unique< cli::Menu >( "subsub" );
        subSubMenu -> Insert(
            "hello",
            [](std::ostream& out){ out << "Hello, subsubmenu world\n"; },
            "Print hello world in the sub-submenu" );
    subMenu -> Insert( std::move(subSubMenu));

    rootMenu -> Insert( std::move(subMenu) );
    rootMenu -> Insert(std::move(menu_placeholder));


    cli::Cli cli( std::move(rootMenu) );
    // global exit action
    cli.ExitAction( [](auto& out){ out << "Goodbye and thanks for all the fish.\n"; } );

    cli::CliFileSession input(cli);
    input.Start();

    return 0;
}

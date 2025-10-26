function showModule(name) {
    // Скрываем все модули
    document.querySelectorAll('.module-content').forEach(m => {
        m.classList.remove('active');
    });

    // Убираем активный класс со всех кнопок
    document.querySelectorAll('#sidebar button').forEach(btn => {
        btn.classList.remove('active');
    });

    // Показываем выбранный модуль
    const target = document.getElementById(name);
    if (target) {
        target.classList.add('active');
    }

    // Активируем соответствующую кнопку
    document.querySelectorAll('#sidebar button').forEach(btn => {
        if (btn.getAttribute('onclick') && btn.getAttribute('onclick').includes("'" + name + "'")) {
            btn.classList.add('active');
        }
    });
}

// При загрузке страницы активируем первый модуль
document.addEventListener('DOMContentLoaded', function () {
    // Проверяем, есть ли уже активный модуль
    const activeModules = document.querySelectorAll('.module-content.active');
    const activeButtons = document.querySelectorAll('#sidebar button.active');

    // Если нет активных, активируем первый
    if (activeModules.length === 0 || activeButtons.length === 0) {
        const firstButton = document.querySelector('#sidebar button');
        if (firstButton && firstButton.getAttribute('onclick')) {
            const onclick = firstButton.getAttribute('onclick');
            const moduleName = onclick.match(/'([^']+)'/)[1];
            showModule(moduleName);
        }
    }
});
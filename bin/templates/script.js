function showModule(name) {
    // Деактивация всех модулей
    document.querySelectorAll('.module-content').forEach(m => {
        m.classList.remove('active');
    });

    // Деактивация всех кнопок
    document.querySelectorAll('#sidebar button').forEach(btn => {
        btn.classList.remove('active');
    });

    // Активация выбранного модуля
    const target = document.getElementById(name);
    if (target) {
        target.classList.add('active');
        initSchematicZoom(target);
    }

    // Активация соответствующей кнопки
    document.querySelectorAll('#sidebar button').forEach(btn => {
        const onclickAttr = btn.getAttribute('onclick');
        if (onclickAttr && onclickAttr.includes(`'${name}'`)) {
            btn.classList.add('active');
        }
    });
}

// Инициализация зума для schematic
function initSchematicZoom(moduleContent) {
    const schematics = moduleContent.querySelectorAll('.module-schema');

    schematics.forEach(schema => {
        schema.removeEventListener('click', handleSchematicClick);
        schema.addEventListener('click', handleSchematicClick);
    });
}

// Обработчик клика по schematic
function handleSchematicClick(e) {
    const schema = e.currentTarget;
    const svg = schema.querySelector('svg');

    if (!svg) return;

    // Создаём модальное окно если его нет
    let modal = document.querySelector('.schematic-modal');
    if (!modal) {
        modal = document.createElement('div');
        modal.className = 'schematic-modal';
        document.body.appendChild(modal);

        modal.addEventListener('click', closeModal);
    }

    // Получаем оригинальные размеры SVG
    const svgWidth = svg.getAttribute('width');
    const svgHeight = svg.getAttribute('height');

    // Клонируем SVG
    const clonedSvg = svg.cloneNode(true);

    // Удаляем атрибуты width и height, оставляем только viewBox
    clonedSvg.removeAttribute('width');
    clonedSvg.removeAttribute('height');
    // viewBox уже есть из оригинала
    // Создаём контейнер для SVG
    const container = document.createElement('div');
    container.className = 'modal-svg-container';
    container.appendChild(clonedSvg);

    // Очищаем и добавляем
    modal.innerHTML = '';
    modal.appendChild(container);

    // Показываем модальное окно
    modal.classList.add('active');

    e.stopPropagation();
}

// Закрытие модального окна
function closeModal() {
    const modal = document.querySelector('.schematic-modal');
    if (modal) {
        modal.classList.remove('active');
        setTimeout(() => {
            modal.innerHTML = '';
        }, 300);
    }
}

// Глобальная инициализация
function initAllSchematics() {
    const activeModule = document.querySelector('.module-content.active');
    if (activeModule) {
        initSchematicZoom(activeModule);
    }
}

function initSchematicZoom(moduleContent) {
    const schematics = moduleContent.querySelectorAll('.module-schema');

    schematics.forEach(schema => {
        const svg = schema.querySelector('svg');
        if (svg) {
            const height = parseInt(svg.getAttribute('height') || '0');
            // Если схема очень высокая, добавляем специальный класс
            if (height > 600) {
                schema.classList.add('large-schematic');
            }
        }

        schema.removeEventListener('click', handleSchematicClick);
        schema.addEventListener('click', handleSchematicClick);
    });
}

// Обработка клавиши ESC
document.addEventListener('keydown', function (e) {
    if (e.key === 'Escape') {
        closeModal();
    }
});

// Предотвращаем закрытие при клике на SVG в модальном окне
document.addEventListener('click', function (e) {
    if (e.target.closest('.schematic-modal svg')) {
        e.stopPropagation();
    }
});

// Инициализация при загрузке
document.addEventListener('DOMContentLoaded', function () {
    const activeModules = document.querySelectorAll('.module-content.active');
    const activeButtons = document.querySelectorAll('#sidebar button.active');

    if (activeModules.length === 0 || activeButtons.length === 0) {
        const firstButton = document.querySelector('#sidebar button');
        if (firstButton) {
            const onclick = firstButton.getAttribute('onclick');
            const match = onclick?.match(/'([^']+)'/);
            if (match) {
                showModule(match[1]);
            }
        }
    }

    initAllSchematics();
});

window.showModule = showModule;
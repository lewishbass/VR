document.addEventListener('DOMContentLoaded', function () {
    const levels = ['ERR ', 'WARN', 'STEP', 'INFO'];
    const levelNames = ['FATAL', 'WARNING', 'STEP', 'INFO'];

    // Extract all unique tags from log entries
    const allTags = new Set();
    document.querySelectorAll('.log-entry').forEach(entry => {
        const tags = entry.getAttribute('data-tags');
        if (tags) {
            tags.trim().split(/\s+/).forEach(tag => {
                if (tag) allTags.add(tag);
            });
        }
    });

    // Create filter container
    const filterContainer = document.createElement('div');
    filterContainer.id = 'filter-container';

    // Level filters
    const levelSection = document.createElement('div');
    levelSection.className = 'filter-section';
    levelSection.innerHTML = '<label>Filter by Level:</label>';
    const levelButtons = document.createElement('div');
    levelButtons.className = 'filter-buttons';

    levels.forEach((level, index) => {
        const btn = document.createElement('button');
        btn.className = 'filter-btn level-filter active';
        btn.setAttribute('data-level', index + 1);
        btn.textContent = levelNames[index];
        levelButtons.appendChild(btn);
    });

    levelSection.appendChild(levelButtons);
    filterContainer.appendChild(levelSection);

    // Tag filters
    if (allTags.size > 0) {
        const tagSection = document.createElement('div');
        tagSection.className = 'filter-section';
        tagSection.innerHTML = '<label>Filter by Tag:</label>';
        const tagButtons = document.createElement('div');
        tagButtons.className = 'filter-buttons';

        allTags.forEach(tag => {
            const btn = document.createElement('button');
            btn.className = 'filter-btn tag-filter active';
            btn.setAttribute('data-tag', tag);
            btn.textContent = tag;
            tagButtons.appendChild(btn);
        });

        tagSection.appendChild(tagButtons);
        filterContainer.appendChild(tagSection);
    }

    // Clear filters button and stats
    const stats = document.createElement('div');
    stats.className = 'stats';
    stats.innerHTML = `<span>Total: <strong id="total-count">0</strong></span><span>Visible: <strong id="visible-count">0</strong></span>`;

    filterContainer.appendChild(stats);

    // Insert filter container after h1
    const h1 = document.querySelector('h1');
    h1.parentNode.insertBefore(filterContainer, h1.nextSibling);

    // Toggle filters
    const activeFilters = {
        levels: new Set([1, 2, 3, 4]),
        tags: new Set() // Start with no tags selected
    };

    document.querySelectorAll('.level-filter').forEach(btn => {
        btn.addEventListener('click', function () {
            const level = parseInt(this.getAttribute('data-level'));
            this.classList.toggle('active');

            if (activeFilters.levels.has(level)) {
                activeFilters.levels.delete(level);
            } else {
                activeFilters.levels.add(level);
            }

            applyFilters();
        });
    });

    document.querySelectorAll('.tag-filter').forEach(btn => {
        btn.addEventListener('click', function () {
            const tag = this.getAttribute('data-tag');
            this.classList.toggle('active');

            if (activeFilters.tags.has(tag)) {
                activeFilters.tags.delete(tag);
            } else {
                activeFilters.tags.add(tag);
            }

            applyFilters();
        });

        // Remove active class by default
        btn.classList.remove('active');
    });

    // Apply filters
    function applyFilters() {
        let visibleCount = 0;

        document.querySelectorAll('.log-entry').forEach(entry => {
            const level = parseInt(entry.className.match(/level-(\d+)/)[1]);
            const entryTags = entry.getAttribute('data-tags').trim().split(/\s+/).filter(t => t);

            const levelMatch = activeFilters.levels.has(level);
            // If no tags are selected, show all entries (by tag criteria)
            // If tags are selected, only show entries that have at least one matching tag
            const tagMatch = activeFilters.tags.size === 0 || entryTags.some(tag => activeFilters.tags.has(tag));

            if (levelMatch && tagMatch) {
                entry.classList.remove('hidden');
                visibleCount++;
            } else {
                entry.classList.add('hidden');
            }
        });

        updateStats(visibleCount);
    }

    // Update stats
    function updateStats(visibleCount) {
        const totalCount = document.querySelectorAll('.log-entry').length;
        document.getElementById('total-count').textContent = totalCount;
        document.getElementById('visible-count').textContent = visibleCount;
    }

    // Expand/collapse log entries
    document.querySelectorAll('.log-entry').forEach(entry => {
        entry.addEventListener('click', function (e) {
            // Don't toggle if clicking on tags
            if (e.target.closest('.log-tags')) return;
            
            const messageEl = this.querySelector('.message');
            if (!messageEl) return;

            const expanded = messageEl.classList.toggle('expanded');

            // Handle <br> tags: hide them when collapsed, show when expanded
            const brTags = messageEl.querySelectorAll('br');
            brTags.forEach(br => {
                br.style.display = expanded ? '' : 'none';
            });

            if (expanded) {
                // Create copy button
                let copyBtn = this.querySelector('.copy-btn');
                if (!copyBtn) {
                    copyBtn = document.createElement('button');
                    copyBtn.className = 'copy-btn';
                    copyBtn.textContent = 'Copy';
                    copyBtn.title = 'Copy to clipboard';
                    copyBtn.type = 'button';
                    copyBtn.setAttribute('aria-label', 'Copy message to clipboard');
                    copyBtn.setAttribute('aria-pressed', 'false');

                    copyBtn.addEventListener('click', (e) => {
                        e.stopPropagation();

                        const fileAttr = this.getAttribute('data-file');
                        const lineAttr = this.getAttribute('data-line');
                        const messageText = messageEl.textContent.trim();

                        const fileName = fileAttr ? fileAttr.split('\\').pop().split('/').pop() : 'unknown';
                        const lineNum = lineAttr && lineAttr !== '-1' ? lineAttr : '?';
                        const copyText = `${fileName}:${lineNum} - ${messageText}`;

                        navigator.clipboard.writeText(copyText).then(() => {
                            copyBtn.textContent = 'Copied!';
                            copyBtn.classList.add('copied');
                            copyBtn.setAttribute('aria-pressed', 'true');

                            setTimeout(() => {
                                copyBtn.textContent = 'Copy';
                                copyBtn.classList.remove('copied');
                                copyBtn.setAttribute('aria-pressed', 'false');
                            }, 1500);
                        }).catch(err => {
                            console.error('Failed to copy:', err);
                        });
                    });

                    this.appendChild(copyBtn);
                }

                // Create file info display
                let fileInfo = this.querySelector('.log-file-info');
                if (!fileInfo) {
                    const fileAttr = this.getAttribute('data-file');
                    const lineAttr = this.getAttribute('data-line');
                    
                    if (fileAttr) {
                        fileInfo = document.createElement('div');
                        fileInfo.className = 'log-file-info';
                        
                        const fileName = fileAttr.split('\\').pop().split('/').pop();
                        const fileSpan = document.createElement('span');
                        fileSpan.className = 'file-name';
                        fileSpan.textContent = fileName;
                        
                        fileInfo.appendChild(fileSpan);
                        
                        if (lineAttr && lineAttr !== '-1') {
                            const separator = document.createTextNode(' : ');
                            fileInfo.appendChild(separator);
                            
                            const lineLink = document.createElement('a');
                            lineLink.className = 'line-link';
                            lineLink.textContent = `Line ${lineAttr}`;
                            
                            // Use vscode:// protocol for VS Code, or file:// as fallback
                            const vscodePath = fileAttr.replace(/\\/g, '/');
                            lineLink.href = `vscode://file/${vscodePath}:${lineAttr}`;
                            lineLink.title = `Open ${fileName} at line ${lineAttr}`;
                            
                            lineLink.addEventListener('click', (e) => {
                                e.stopPropagation();
                            });
                            
                            fileInfo.appendChild(lineLink);
                        }
                        
                        fileInfo.addEventListener('click', (e) => {
                            e.stopPropagation();
                        });
                        
                        this.appendChild(fileInfo);
                    }
                }

                // create tag container lazily if missing
                let tagContainer = this.querySelector('.log-tags');
                if (!tagContainer) {
                    const tagsAttr = this.getAttribute('data-tags') || '';
                    const tags = tagsAttr.trim().split(/\s+/).filter(t => t);

                    tagContainer = document.createElement('div');
                    tagContainer.className = 'log-tags';

                    if (tags.length > 0) {
                        tags.forEach(tag => {
                            const span = document.createElement('span');
                            span.className = 'log-tag';
                            span.textContent = tag;
                            tagContainer.appendChild(span);
                        });
                    } else {
                        const span = document.createElement('span');
                        span.className = 'log-tag empty';
                        span.textContent = 'No tags';
                        tagContainer.appendChild(span);
                    }

                    // Clicking tags shouldn't bubble and re-toggle expansion
                    tagContainer.addEventListener('click', (e) => {
                        e.stopPropagation();
                    });

                    // Append tags to the log-entry (will appear on new line due to flex-basis)
                    this.appendChild(tagContainer);
                }
            }
        });

        // Initialize: hide <br> tags by default
        const messageEl = entry.querySelector('.message');
        if (messageEl) {
            const brTags = messageEl.querySelectorAll('br');
            brTags.forEach(br => {
                br.style.display = 'none';
            });
        }
    });

    // Initial stats
    updateStats(document.querySelectorAll('.log-entry').length);
});